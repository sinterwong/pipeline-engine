/**
 * @file smokingModule.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-07-31
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "smokingModule.h"
#include <cstdlib>
#include <experimental/filesystem>
#include <sys/stat.h>
#include <unistd.h>

namespace module {

SmokingModule::SmokingModule(Backend *ptr, const std::string &initName,
                             const std::string &initType,
                             const common::LogicConfig &logicConfig,
                             const std::vector<std::string> &recv,
                             const std::vector<std::string> &send)
    : LogicModule(ptr, initName, initType, logicConfig, recv, send) {}

/**
 * @brief
 * 1. recv 类型：stream, algorithm
 * 2. send 类型：algorithm, output
 *
 * @param message
 */
void SmokingModule::forward(std::vector<forwardMessage> message) {
  if (recvModule.empty()) {
    return;
  }
  for (auto &[send, type, buf] : message) {
    if (type == "ControlMessage") {
      // FLOWENGINE_LOGGER_INFO("{} SmokingModule module was done!", name);
      std::cout << name << "{} SmokingModule module was done!" << std::endl;
      stopFlag.store(true);
      if (outputStream && outputStream->IsStreaming()) {
        outputStream->Close();
      }
      return;
    }
    if (isRecord) {
      if (type == "stream") {
        recordVideo(buf.key, buf.cameraResult.widthPixel,
                    buf.cameraResult.heightPixel);
      }
      continue;
    }

    if (type == "algorithm") {
      // 此处根据 buf.algorithmResult 写吸烟的逻辑并填充 buf.alarmResult 信息
      // 如果符合条件就发送至AlarmOutputModule
      for (int i = 0; i < buf.algorithmResult.bboxes.size(); i++) {
        auto &bbox = buf.algorithmResult.bboxes.at(i);
        if (bbox.first != send) {
          continue;
        }
        // std::cout << "classid: " << bbox.second.at(5) << ", "
        //           << "confidence: " << bbox.second.at(4) << std::endl;
        if (bbox.second.at(5) == 1 && bbox.second.at(4) > 0.9) {
          // 生成报警信息和报警图
          generateAlarm(buf, "存在吸烟行为", bbox);

          // 发送至后端
          sendWithTypes(buf, {"output"});

          // 保存视频
          if (params.videDuration > 0) {
            initRecord(buf);
          }
        }
        break;
      }
    } else if (type == "stream") {
      // 配置算法推理时需要用到的信息
      buf.logicInfo.region = params.region;
      buf.logicInfo.attentionClasses = params.attentionClasses;
      // 不能发送给output
      sendWithTypes(buf, {"algorithm"});
    }
  }
}

FlowEngineModuleRegister(SmokingModule, Backend *, std::string const &,
                         std::string const &, common::LogicConfig const &,
                         std::vector<std::string> const &,
                         std::vector<std::string> const &);
} // namespace module