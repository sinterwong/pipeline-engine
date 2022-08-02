/**
 * @file config.h
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-05-30
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __FLOWENGINE_PARSER_CONFIG_H_
#define __FLOWENGINE_PARSER_CONFIG_H_
#include <array>
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <unordered_map>

#include "common/common.hpp"
#include "logger/logger.hpp"

#include "rapidjson/document.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace utils {

using common::ModuleConfigure;
using common::ParamsConfig;

static size_t curl_callback(void *ptr, size_t size, size_t nmemb,
                            std::string *data) {
  data->append((char *)ptr, size * nmemb);
  return size * nmemb;
}

class ConfigParser {
private:
  // 配置参数类型
  std::unordered_map<std::string, common::ConfigType> typeMapping {
    std::make_pair("stream", common::ConfigType::Stream),
    std::make_pair("detection", common::ConfigType::Algorithm),
    std::make_pair("classifier", common::ConfigType::Algorithm),
    std::make_pair("output", common::ConfigType::Output),
    std::make_pair("logic", common::ConfigType::Logic),
  };
  // 启动模块的类型
  std::unordered_map<std::string, common::ModuleType> moduleMapping {
    std::make_pair("stream", common::ModuleType::Stream),
    std::make_pair("detection", common::ModuleType::Detection),
    std::make_pair("classifier", common::ModuleType::Classifier),
    std::make_pair("output", common::ModuleType::Output),
    std::make_pair("calling", common::ModuleType::Calling),
    std::make_pair("smoking", common::ModuleType::Smokeing)
  };


public:
  // ConfigParser(std::string const &url_) : url(url_) {}
  bool
  parseConfig(const char *jsonstr,
              std::vector<std::vector<std::pair<ModuleConfigure, ParamsConfig>>>
                  &pipelinesConfigs);

  bool readFile(std::string const &filename, std::string &result);

  bool writeJson(std::string const &config, std::string const &outPath);

  bool postConfig(std::string const &url, int deviceId, std::string &result);
};

} // namespace utils
#endif
