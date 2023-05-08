/**
 * @file xDecoder.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-01-05
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __DECODER_FOR_X3_H_
#define __DECODER_FOR_X3_H_

#include "ffstream.hpp"
#include "joining_thread.h"
#include "logger/logger.hpp"
#include "videoSource.hpp"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <shared_mutex>
#include <sp_codec.h>
#include <sp_vio.h>

#include <chrono>
#include <thread>
#include <utility>
using namespace std::chrono_literals;
namespace video {

class XDecoder : public videoSource {

public:
  /**
   * Create a decoder from the provided video options.
   */
  static std::unique_ptr<XDecoder> Create(videoOptions const &options) {
    std::unique_ptr<XDecoder> cam =
        std::unique_ptr<XDecoder>(new XDecoder(options));
    if (!cam) {
      return nullptr;
    }
    // initialize decoder (with fallback)
    if (!cam->Init()) {
      FLOWENGINE_LOGGER_ERROR("XDecoder -- failed to create device!");
      return nullptr;
    }
    FLOWENGINE_LOGGER_INFO("XDecoder -- successfully created device!");
    return cam;
  }

  /**
   * Destructor
   */
  ~XDecoder() {
    if (mStreaming.load()) {
      Close();
    }
    sp_release_decoder_module(decoder);
  };

  virtual bool Open() override {
    // 启动流
    stream = std::make_unique<FFStream>(mOptions.resource);
    if (!stream->openStream()) {
      FLOWENGINE_LOGGER_ERROR("can't open the stream {}!",
                              std::string(mOptions.resource));
      return false;
    }
    mOptions.width = stream->getWidth();
    mOptions.height = stream->getHeight();
    mOptions.frameRate = stream->getRate();
    FLOWENGINE_LOGGER_INFO("{} video is opened!", mOptions.resource.string);

    int ret = sp_start_decode(decoder, "", mOptions.videoIdx,
                              entypeMapping.at(stream->getCodecType()),
                              stream->getWidth(), stream->getHeight());
    if (ret != 0) {
      FLOWENGINE_LOGGER_ERROR("sp_open_decoder failed {}!", ret);
      return false;
    }
    FLOWENGINE_LOGGER_INFO("sp_open_decoder is successed!");
    int yuv_size = FRAME_BUFFER_SIZE(mOptions.width, mOptions.height);
    yuv_data = reinterpret_cast<char *>(malloc(yuv_size * sizeof(char)));
    // raw_data = malloc(mOptions.width * mOptions.height * 3 * sizeof(char));
    mStreaming.store(true);
    producter = std::make_unique<joining_thread>([this]() {
      // 优化成一个条件变量
      while (true) {
        std::this_thread::sleep_for(10ms);
        int bufSize = stream->getRawFrame(&raw_data);
        if (bufSize < 0)
          break;
        int ret =
            sp_decoder_set_image(decoder, reinterpret_cast<char *>(raw_data),
                                 mOptions.videoIdx, bufSize, 0);
        if (ret != 0) {
          FLOWENGINE_LOGGER_WARN("sp_decoder_set_image is failed: {}", ret);
          std::this_thread::sleep_for(2s);
          continue;
        }
      }
      FLOWENGINE_LOGGER_WARN("streaming is over: {}",
                             std::string(mOptions.resource));
      Close();
    });
    return true;
  };

  /**
   * Close the stream.
   * @see videoSource::Close()
   */
  virtual inline void Close() noexcept override {
    if (stream->isRunning()) {
      stream->closeStream();
    }
    mStreaming.store(false);
    sp_stop_decode(decoder);
    free(yuv_data);
  }

  virtual inline size_t GetWidth() const noexcept override {
    return stream->getWidth();
  }

  virtual inline size_t GetHeight() const noexcept override {
    return stream->getHeight();
  }

  virtual inline size_t GetFrameRate() const noexcept override {
    return stream->getRate();
  }

  virtual bool Capture(void **image,
                       size_t timeout = DEFAULT_TIMEOUT) override {
    int ret = sp_decoder_get_image(decoder, yuv_data);
    if (ret != 0) {
      FLOWENGINE_LOGGER_WARN("sp_decoder_get_image get next frame is failed!");
      return false;
    }
    // TODO 数据如何给出去? copy? 先写出来吧，这样是不安全的
    *image = reinterpret_cast<void *>(yuv_data);
    return true;
  }

  /**
   * Return the interface type
   */
  virtual inline size_t GetType() const noexcept override { return Type; }

  /**
   * Unique type identifier of decoder class.
   */
  static const size_t Type = (1 << 0);

private:
  XDecoder(videoOptions const &options) : videoSource(options) {}
  const std::unordered_map<std::string, int> entypeMapping{
      std::pair<std::string, int>("h264", SP_ENCODER_H264),
      std::pair<std::string, int>("h265", SP_ENCODER_H265),
      std::pair<std::string, int>("mpeg", SP_ENCODER_MJPEG),
  };
  std::unique_ptr<FFStream> stream;
  void *decoder;
  char *yuv_data;
  void *raw_data;

  virtual bool Init() override {
    decoder = sp_init_decoder_module();
    return true;
  }
  std::unique_ptr<joining_thread> producter;
};
} // namespace video
#endif