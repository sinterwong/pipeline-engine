/**
 * @file pipelineModule.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-07-10
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __METAENGINE_STATUE_CONTROL_H
#define __METAENGINE_STATUE_CONTROL_H

#include "backend.h"
#include "boostMessage.h"
#include "common/common.hpp"
#include "logger/logger.hpp"
#include "module.hpp"
#include "thread_pool.h"
#include "utils/configParser.hpp"
// #include "BS_thread_pool.hpp"

#include <algorithm>
#include <any>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

namespace module {
using common::ModuleConfigure;
using common::ParamsConfig;

class PipelineModule {
private:
  std::string name = "Control";
  std::string type = "ControlMessage";
  std::string config;
  utils::ConfigParser configParser;
  Backend backend{std::make_unique<BoostMessage>(),
                  std::make_unique<RouteFramePool>(2)};
  // std::unique_ptr<thread_pool> pool;
  std::unique_ptr<thread_pool> pool;
  std::unordered_map<std::string, std::shared_ptr<Module>> atm;

private:
  /**
   * @brief 提交模块到线程池
   *
   * @param config
   * @param paramsConfig
   * @return true
   * @return false
   */
  bool submitModule(common::ModuleConfigure const &config,
                    common::ParamsConfig const &paramsConfig);

  /**
   * @brief 终止并删除模块
   *
   * @param moduleName
   */
  void stopModule(std::string const &moduleName);

  /**
   * @brief 关联模块
   *
   * @param moduleName
   * @param sendModule
   * @param recvModule
   */
  void attachModule(std::string const &moduleName,
                    std::string const &sendModule,
                    std::string const &recvModule);

  /**
   * @brief 解除模块关联
   *
   * @param moduleName
   */
  void detachModule(std::string const &moduleName);

  bool startPipeline();

  bool parseConfigs(
      std::string const &uri,
      std::vector<std::vector<std::pair<ModuleConfigure, ParamsConfig>>> &);

  void terminate() {
    // 向所有模块发送终止信号
    for (auto iter = atm.begin(); iter != atm.end(); ++iter) {
      backend.message->send(name, iter->first, type, queueMessage());
      // iter->second->stopFlag.store(true);
    }
  }

public:
  PipelineModule(std::string const &config_, size_t workers_n);

  ~PipelineModule() { terminate(); }

  bool initialize(); // 第一次读取配置文件并且启动流资源

  void run(); // 开始运行
};
} // namespace module
#endif // __METAENGINE_STATUE_CONTROL_H
