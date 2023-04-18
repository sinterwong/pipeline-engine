/**
 * @file objectNumber.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "objectNumberModule.h"
#include "logger/logger.hpp"

#include <cassert>

namespace module {

/**
 * @brief
 *
 * @param message
 */
void ObjectNumberModule::forward(std::vector<forwardMessage> &message) {
  for (auto &[send, type, buf] : message) {
    if (type == MessageType::Close) {
      FLOWENGINE_LOGGER_INFO("{} HelmetModule module was done!", name);
      stopFlag.store(true);
      return;
    }

    // 读取图片
    FrameBuf frameBufMessage = ptr->pool->read(buf.key);
    auto image =
        std::any_cast<std::shared_ptr<cv::Mat>>(frameBufMessage.read("Mat"));

    // 初始待计算区域，每次算法结果出来之后需要更新regions
    std::vector<common::RetBox> regions;
    for (auto const &area : config->regions) {
      regions.emplace_back(common::RetBox{
          name,
          {static_cast<float>(area[0].x), static_cast<float>(area[0].y),
           static_cast<float>(area[1].x), static_cast<float>(area[1].y), 0.0,
           0.0}});
    }
    if (regions.empty()) {
      // 前端没有画框
      regions.emplace_back(common::RetBox{name, {0, 0, 0, 0, 0, 0}});
    }

    // 根据提供的配置执行算法，
    auto &detNet = config->algoPipelines.at(0);
    auto &attentions = detNet.second.attentions; // 检测关注的类别

    int objectNumber = 0;
    for (auto &region : regions) {

      InferParams detParams{name,
                            buf.frameType,
                            detNet.second.cropScaling,
                            region,
                            {image->cols, image->rows, image->channels()}};

      InferResult detRet;
      if (!ptr->algo->infer(detNet.first, image->data, detParams, detRet)) {
        return;
      };
      // 获取检测结果信息
      auto bboxes = std::get_if<common::BBoxes>(&detRet.aRet);
      if (!bboxes) {
        FLOWENGINE_LOGGER_ERROR("ObjectNumberModule: Wrong algorithm type!");
        return;
      }

      for (auto &bbox : *bboxes) {
        // 需要过滤掉不关注的类别
        if (!attentions.empty()) {
          auto iter = std::find(attentions.begin(), attentions.end(),
                                static_cast<int>(bbox.class_id));
          if (iter == attentions.end()) { // 该类别没有出现在关注类别中
            continue;
          }
        }
        objectNumber++;
      }
    }

    FLOWENGINE_LOGGER_CRITICAL("object number: {}", objectNumber);

    // 每到一定的数量就会触发报警
    if (objectNumber >= config->amount) {
      // 生成报警信息
      alarmUtils.generateAlarmInfo(name, buf.alarmInfo, "数量达标",
                                   config.get());
      // 生成报警图片，此处相当于截了个图
      alarmUtils.saveAlarmImage(buf.alarmInfo.alarmFile + "/" +
                                    buf.alarmInfo.alarmId + ".jpg",
                                *image, buf.frameType, config->isDraw);
      autoSend(buf);
    }
  }
}

FlowEngineModuleRegister(ObjectNumberModule, backend_ptr, std::string const &,
                         MessageType const &, ModuleConfig &);
} // namespace module
