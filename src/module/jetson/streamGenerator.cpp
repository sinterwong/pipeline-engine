/**
 * @file jetsonSourceModule.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-06-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "streamGenerator.h"
#include <opencv2/imgproc.hpp>

namespace module {

void delBuffer(std::vector<std::any> &list) {
  assert(list.size() == 1);
  assert(list[0].has_value());
  assert(list[0].type() == typeid(uchar3 *));
  list.clear();
}

std::any getPtrBuffer(std::vector<std::any> &list, FrameBuf *buf) {
  assert(list.size() == 1);
  assert(list[0].has_value());
  assert(list[0].type() == typeid(uchar3 *));
  return reinterpret_cast<void *>(std::any_cast<uchar3 *>(list[0]));
}

std::any getMatBuffer(std::vector<std::any> &list, FrameBuf *buf) {
  assert(list.size() == 1);
  assert(list[0].has_value());
  assert(list[0].type() == typeid(uchar3 *));

  void *data = reinterpret_cast<void *>(std::any_cast<uchar3 *>(list[0]));
  std::shared_ptr<cv::Mat> mat =
      std::make_shared<cv::Mat>(buf->height, buf->width, CV_8UC3, data);
  // cv::cvtColor(*mat, *mat, cv::COLOR_BGR2RGB);
  return mat;
}

std::any getUchar3Buffer(std::vector<std::any> &list, FrameBuf *buf) {
  assert(list.size() == 1);
  assert(list[0].has_value());
  assert(list[0].type() == typeid(uchar3 *));

  return std::any_cast<uchar3 *>(list[0]);
}

FrameBuf makeFrameBuf(uchar3 *image, int height, int width) {
  FrameBuf temp;
  temp.write({std::make_any<uchar3 *>(image)},
             {std::make_pair("void*", getPtrBuffer),
              std::make_pair("Mat", getMatBuffer),
              std::make_pair("uchar3*", getUchar3Buffer)},
             delBuffer, std::make_tuple(width, height, 3, UINT8));
  return temp;
}

StreamGenerator::StreamGenerator(Backend *ptr, const std::string &initName,
                                 const std::string &initType,
                                 const common::CameraConfig &_params)
    : Module(ptr, initName, initType), params(_params) {

  opt.height = params.heightPixel;
  opt.width = params.widthPixel;
  opt.codec = videoOptions::CodecFromStr(params.videoCode.c_str());
  opt.resource = params.cameraIp;
  inputStream = std::unique_ptr<videoSource>(videoSource::Create(opt));
  // inputStream = videoSource::Create(opt);
  if (!inputStream) {
    LogError("jetson source:  failed to create input stream\n");
    exit(-1);
  }
  cameraResult = CameraResult{static_cast<int>(inputStream->GetWidth()),
                              static_cast<int>(inputStream->GetHeight()),
                              static_cast<int>(inputStream->GetFrameRate()),
                              params.cameraId,
                              params.videoCode,
                              params.flowType,
                              params.cameraIp};
}

void StreamGenerator::step() {
  message.clear();
  hash.clear();
  loop = false;

  beforeGetMessage();
  beforeForward();

  forward(message);
  afterForward();
}

void StreamGenerator::forward(std::vector<forwardMessage> &message) {
  for (auto &[send, type, buf] : message) {
    if (type == "ControlMessage") {
      // FLOWENGINE_LOGGER_INFO("{} StreamGenerator module was done!", name);
      std::cout << name << "{} JetsonSource module was done!" << std::endl;
      inputStream->Close();
      stopFlag.store(true);
      return;
    }
  }
  assert(inputStream);
  bool ret = inputStream->Capture(&frame, 1000);

  queueMessage sendMessage;

  if (!ret) {
    if (!inputStream->IsStreaming()) {
      LogInfo("jetson source:  input steram was done.\n");
      sendMessage.status = 2;
      stopFlag.store(true);
    }
  } else {
    FrameBuf frameBufMessage =
        makeFrameBuf(frame, inputStream->GetHeight(), inputStream->GetWidth());
    int returnKey = backendPtr->pool->write(frameBufMessage);
    sendMessage.frameType = ColorType::RGB888;
    sendMessage.key = returnKey;
    sendMessage.cameraResult = cameraResult;
    sendMessage.status = 0;
    autoSend(sendMessage);
  }
}

FlowEngineModuleRegister(StreamGenerator, Backend *, std::string const &,
                         std::string const &, common::CameraConfig const &);
} // namespace module