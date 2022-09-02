//
// Created by Wallel on 2022/2/22.
//

#ifndef METAENGINE_OPENCVCAMERAMODULE_H
#define METAENGINE_OPENCVCAMERAMODULE_H

#include <any>
#include <memory>
#include <opencv2/opencv.hpp>
#include <vector>

#include "basicMessage.pb.h"
#include "frameMessage.pb.h"
#include "module.hpp"

namespace module {
void delBuffer(std::vector<std::any> list);
std::any getPtrBuffer(std::vector<std::any> &list, FrameBuf *buf);
std::any getBuffer(std::vector<std::any> &list, FrameBuf *buf);
FrameBuf makeFrameBuf(std::shared_ptr<cv::Mat> mat);

class OpencvCameraModule : public Module {
private:
  std::string fileName;
  int cameraNumber;
  bool readFile;

  cv::VideoCapture cap;
  std::shared_ptr<cv::Mat> frame;
  bool ret;
  tutorial::FrameMessage buf;

public:
  OpencvCameraModule(Backend *ptr, const std::string &fileName,
                     const std::string &initName, const std::string &initType,
                     const std::vector<std::string> &recv = {},
                     const std::vector<std::string> &send = {});

  OpencvCameraModule(Backend *ptr, const int capNumber,
                     const std::string &initName, const std::string &initType,
                     const std::vector<std::string> &recv = {},
                     const std::vector<std::string> &send = {});

  void forward(std::vector<forwardMessage> message) override;

  void afterForward() override;
};
} // namespace module
#endif // METAENGINE_OPENCVCAMERAMODULE_H
