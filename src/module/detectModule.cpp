/**
 * @file detectModule.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-06-03
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "detectModule.h"
#include "infer_utils.hpp"
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <opencv2/core/mat.hpp>

namespace module {

DetectModule::DetectModule(Backend *ptr, const std::string &initName,
                           const std::string &initType,
                           const common::AlgorithmConfig &_params)
    : Module(ptr, initName, initType), params(std::move(_params)) {

  instance = std::make_shared<AlgoInference>(params);
  instance->initialize();

  instance->getModelInfo(modelInfo);

  detector = ObjectFactory::createObject<infer::vision::Detection>(
      params.algorithmSerial, params, modelInfo);
  // if (params.algorithmSerial == "yolo") {
  //   detector = std::make_shared<infer::vision::Yolo>(params, modelInfo);
  // } else if (params.algorithmSerial == "assd") {
  //   detector = std::make_shared<infer::vision::Assd>(params, modelInfo);
  // } else {
  //   FLOWENGINE_LOGGER_ERROR("Error algorithm serial {}",
  //                           params.algorithmSerial);
  // }
  if (detector == nullptr) {
    FLOWENGINE_LOGGER_ERROR("Error algorithm serial {}",
                            params.algorithmSerial);
    exit(-1);
  }
}

DetectModule::~DetectModule() {}

void DetectModule::forward(std::vector<forwardMessage> &message) {
  if (!instance) {
    std::cout << "instance is not init!!!" << std::endl;
    return;
  }
  for (auto &[send, type, buf] : message) {
    if (type == "ControlMessage") {
      // FLOWENGINE_LOGGER_INFO("{} DetectModule module was done!", name);
      std::cout << name << "{} Detection module was done!" << std::endl;
      stopFlag.store(true);
      return;
    }
    auto frameBufMessage = backendPtr->pool->read(buf.key);

    std::shared_ptr<cv::Mat> image =
        std::any_cast<std::shared_ptr<cv::Mat>>(frameBufMessage.read("Mat"));

    if (type == "logic") {
      if (count++ < 5) {
        return;
      }
      count = 0;
      cv::Rect2i region{
          buf.logicInfo.region[0], buf.logicInfo.region[1],
          buf.logicInfo.region[2] - buf.logicInfo.region[0],
          buf.logicInfo.region[3] - buf.logicInfo.region[1]};
      cv::Mat inferImage;
      if (region.area() != 0) {
        infer::utils::cropImage(*image, inferImage, region, buf.frameType);
      } else {
        inferImage = image->clone();
      }
      infer::Result ret;
      ret.shape = {inferImage.cols, inferImage.rows, 3};
      void *outputs[modelInfo.output_count];
      void *output = reinterpret_cast<void *>(outputs);
      infer::FrameInfo frame;
      frame.data = reinterpret_cast<void **>(&inferImage.data);
      frame.shape = ret.shape;
      if (!instance->infer(frame, &output)) {
        continue;
      }

      detector->processOutput(&output, ret);

      for (auto &rbbox : ret.detResults) {
        // retBox b{}
        retBox b = {name,
                    {rbbox.bbox[0] + region.x, rbbox.bbox[1] + region.y,
                     rbbox.bbox[2] + region.x, rbbox.bbox[3] + region.y,
                     rbbox.class_confidence, rbbox.class_id}};
        buf.algorithmResult.bboxes.emplace_back(std::move(b));
      }
    } else if (type == "algorithm") {
      cv::Mat inferImage;
      for (size_t i = 0; i < buf.algorithmResult.bboxes.size(); i++) {
        auto &bbox = buf.algorithmResult.bboxes.at(i);
        if (bbox.first == send) {
          cv::Rect2i rect{static_cast<int>(bbox.second[0]),
                          static_cast<int>(bbox.second[1]),
                          static_cast<int>(bbox.second[2] - bbox.second[0]),
                          static_cast<int>(bbox.second[3] - bbox.second[1])};
          infer::utils::cropImage(*image, inferImage, rect, buf.frameType);
          infer::Result ret;
          ret.shape = {inferImage.cols, inferImage.rows, 3};
          infer::FrameInfo frame;
          frame.data = reinterpret_cast<void **>(&inferImage.data);
          frame.shape = ret.shape;
          void *outputs[modelInfo.output_count];
          void *output = reinterpret_cast<void *>(outputs);
          if (!instance->infer(frame, &output)) {
            continue;
          }
          detector->processOutput(&output, ret);
          for (auto &rbbox : ret.detResults) {
            retBox b = {
                name,
                {rbbox.bbox[0] + bbox.second[0], rbbox.bbox[1] + bbox.second[1],
                 rbbox.bbox[2] + bbox.second[0], rbbox.bbox[3] + bbox.second[1],
                 rbbox.class_confidence, rbbox.class_id}};
            buf.algorithmResult.bboxes.emplace_back(std::move(b));
          }
        }
      }
    }
    autoSend(buf);
  }
}
FlowEngineModuleRegister(DetectModule, Backend *, std::string const &,
                         std::string const &, common::AlgorithmConfig const &);
} // namespace module