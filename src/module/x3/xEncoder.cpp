/**
 * @file XEncoder.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-01-12
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "x3/xEncoder.hpp"
#include "logger/logger.hpp"
#include <fstream>
#include <memory>
#include <sp_codec.h>
#include <sp_vio.h>

#define STREAM_FRAME_SIZE 2097152 // 2^21

namespace module::utils {
std::unique_ptr<XEncoder> XEncoder::create(videoOptions const &options) {
  return std::unique_ptr<XEncoder>(new XEncoder(options));
}

XEncoder::~XEncoder() {
  close();
  sp_release_encoder_module(encoder);
}

bool XEncoder::init() {
  encoder = sp_init_encoder_module();
  stream_buffer =
      reinterpret_cast<char *>(malloc(sizeof(char) * STREAM_FRAME_SIZE));
  return true;
}

bool XEncoder::open() noexcept {
  // chn 做成全局变量
  int ret = sp_start_encode(encoder, mOptions.videoIdx, SP_ENCODER_H264,
                            mOptions.width, mOptions.height, 8000);
  if (ret != 0) {
    FLOWENGINE_LOGGER_ERROR("sp_open_encode failed {}!", ret);
    return false;
  }
  outStream = std::ofstream(mOptions.resource.location,
                            std::ios::out | std::ios::binary);
  FLOWENGINE_LOGGER_INFO("sp_open_encode is successed!");
  mStreaming.store(true);
  return true;
}

bool XEncoder::close() noexcept {
  outStream.close();
  sp_stop_encode(encoder);
  free(stream_buffer);
  stream_buffer = nullptr;
  mStreaming.store(false);
  return true;
}

bool XEncoder::render(void **image) {
  if (!mStreaming.load())
    if (!open())
      return false;
  sp_encoder_set_frame(encoder, reinterpret_cast<char *>(*image), frame_size);

  memset(reinterpret_cast<void *>(stream_buffer), 0, STREAM_FRAME_SIZE);
  int stream_frame_size = sp_encoder_get_stream(encoder, stream_buffer);
  if (stream_frame_size == -1) {
    FLOWENGINE_LOGGER_ERROR("sp_encoder_get_stream error!");
    close();
    return false;
  }
  outStream.write(stream_buffer, stream_frame_size);
  return true;
}

} // namespace module::utils
