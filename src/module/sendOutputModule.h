/**
 * @file sendOutputModule.h
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-06-05
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __METAENGINE_SEND_OUTPUT_H_
#define __METAENGINE_SEND_OUTPUT_H_

#include <any>
#include <curl/curl.h>
#include <memory>
#include <opencv2/opencv.hpp>
#include <random>
#include <vector>

#include "messageBus.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "common/common.hpp"
#include "logger/logger.hpp"
#include "module.hpp"
#include "utils/convertMat.hpp"

namespace module {

struct AlarmInfo {
  int height;                  // 高
  int width;                   // 宽
  int cameraId;                // 摄像机 ID
  std::string cameraIp;        // 视频流 IP
  std::string alarmType;       // 报警类型
  std::string alarmFile;       // 报警图片(base64)
  std::string alarmId;         // 本次报警唯一 ID
  std::string alarmDetails;    // 报警细节
  std::string algorithmResult; // 算法返回结果
};

class SendOutputModule : public Module {
private:
  bool ret;
  utils::ImageConverter imageConverter;
  std::string url;
  int count = 0;

public:
  SendOutputModule(Backend *ptr, const std::string &initName,
                   const std::string &initType,
                   const common::OutputConfig &outputConfig,
                   const std::vector<std::string> &recv = {},
                   const std::vector<std::string> &send = {},
                   const std::vector<std::string> &pool = {});
  ~SendOutputModule() {}

  void forward(std::vector<std::tuple<std::string, std::string, queueMessage>>
                   message) override;

  bool postResult(std::string const &url, AlarmInfo const &resultInfo,
                  std::string &result);

  bool writeResult(AlgorithmResult const &rm, std::string &result);

  bool drawResult(cv::Mat &image, AlgorithmResult const &rm);

  inline unsigned int random_char() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    return dis(gen);
  }

  inline std::string generate_hex(const unsigned int len) {
    std::stringstream ss;
    for (auto i = 0; i < len; i++) {
      const auto rc = random_char();
      std::stringstream hexstream;
      hexstream << std::hex << rc;
      auto hex = hexstream.str();
      ss << (hex.length() < 2 ? '0' + hex : hex);
    }
    return ss.str();
  }
};
} // namespace module
#endif // __METAENGINE_SEND_OUTPUT_H_
