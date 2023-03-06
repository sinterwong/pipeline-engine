/**
 * @file pipelineModule.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-07-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "pipeline.hpp"
#include "factory.hpp"
#include "logger/logger.hpp"
#include "module.hpp"
#include <algorithm>
#include <memory>
#include <unordered_map>
#include <vector>

#include "module_utils.hpp"

namespace module {

PipelineModule::PipelineModule(std::string const &config_path_, size_t workers_n)
    : config_path(config_path_) {
  pool = std::unique_ptr<thread_pool>{std::make_unique<thread_pool>()};
  pool->start(workers_n);
}

bool PipelineModule::submitModule(ModuleInfo const &info,
                                  ModuleConfig const &config) {
  atm[info.moduleName] = ObjectFactory::createObject<Module>(
      info.className, backendPtr, info.moduleName, info.moduleType, config);

  if (atm[info.moduleName] == nullptr) {
    FLOWENGINE_LOGGER_ERROR("Module {} fails to be started!", info.moduleName);
    return false;
  }
  atm[info.moduleName]->addRecvModule(name); // 关联管理员模块
  pool->submit(&Module::go, atm.at(info.moduleName));
  return true;
}

bool PipelineModule::parseConfigs(std::string const &path,
                                  std::vector<PipelineParams> &pipelines) {
  std::string content;
  if (!utils::readFile(path, content)) {
    FLOWENGINE_LOGGER_ERROR("config parse: read file is failed!");
    return false;
  }

  if (!configParser.parseConfig(content.c_str(), pipelines)) {
    FLOWENGINE_LOGGER_INFO("config parse: parseParams is failed!");
    return false;
  }

  if (!utils::writeJson("{}", path)) {
    FLOWENGINE_LOGGER_INFO("config parse: clean json file is failed!");
    return false;
  }
  return true;
}

void PipelineModule::attachModule(std::string const &moduleName,
                                  std::string const &sendModule,
                                  std::string const &recvModule) {
  if (!sendModule.empty()) {
    atm.at(moduleName)->addSendModule(sendModule);
    atm.at(sendModule)->addRecvModule(moduleName);
  }

  if (!recvModule.empty()) {
    atm.at(moduleName)->addRecvModule(recvModule);
    atm.at(recvModule)->addSendModule(moduleName);
  }
}

void PipelineModule::detachModule(std::string const &moduleName) {
  // 找到并解除输入模块的所有关联
  auto iter = atm.find(moduleName);
  if (iter == atm.end()) {
    FLOWENGINE_LOGGER_ERROR("{} is not runing", moduleName);
    return;
  }
  for (auto &sm : atm.at(moduleName)->getSendModule()) {
    atm.at(sm)->delRecvModule(moduleName);
  };

  for (auto &rm : atm.at(moduleName)->getRecvModule()) {
    if (rm == name) { // 保留pipeline控制模块
      continue;
    }
    atm.at(rm)->delSendModule(moduleName);
  };
}

void PipelineModule::stopModule(std::string const &moduleName) {

  // 解除关联
  detachModule(moduleName);

  // 发送终止正在 go 的消息
  backendPtr->message->send(name, moduleName, MessageType::Close,
                            queueMessage());

  // 从atm中删除对象
  atm.erase(moduleName);
}

bool PipelineModule::startPipeline() {
  // 开始之前 先检查目前存在的模块状态是否已经停止（针对摄像头等外部因素）
  std::unordered_map<std::string, std::shared_ptr<Module>>::iterator iter;
  for (iter = atm.begin(); iter != atm.end();) {
    if (iter->second->stopFlag.load()) {
      // 该模块已经停止
      iter = atm.erase(iter);
    } else {
      ++iter;
    }
  }

  std::vector<PipelineParams> pipelines;
  if (!parseConfigs(config_path, pipelines)) {
    FLOWENGINE_LOGGER_ERROR("parse config error");
    return false;
  }
  std::vector<std::string> currentModules;
  std::vector<ModuleInfo> moduleRelations;
  // run 起来所有模块并且制作所有需要关联的模块, 后续可能会有所扩展
  for (auto &pipeline : pipelines) {
    for (auto &config : pipeline) {
      currentModules.push_back(config.first.moduleName);
      moduleRelations.push_back(config.first);
      if (atm.find(config.first.moduleName) == atm.end()) {
        submitModule(config.first, config.second);
      }
    }
  }
  // 清掉已经停用的模块
  if (!currentModules.empty()) {
    std::unordered_map<std::string, std::shared_ptr<Module>>::iterator iter;
    for (iter = atm.begin(); iter != atm.end();) {
      auto it =
          std::find(currentModules.begin(), currentModules.end(), iter->first);
      if (it == currentModules.end()) {
        // 说明该模块需要关闭（函数中存在删除atm的部分）
        // 解除关联
        detachModule(iter->first);
        // 发送终止正在 go 的消息
        // iter->second->stopFlag.store(true);
        backendPtr->message->send(name, iter->first, MessageType::Close,
                                  queueMessage());
        // 从atm中删除对象
        iter = atm.erase(iter);
      } else {
        ++iter;
      }
    }

    // 关联模块
    for (auto &mc : moduleRelations) {
      attachModule(mc.moduleName, mc.sendName, mc.recvName);
    }
  }

  return true;
}

void PipelineModule::run() {
  while (true) {
    startPipeline();
    std::this_thread::sleep_for(std::chrono::seconds(30));
    // terminate();  // 终止所有任务
    // break;
  }
}

} // namespace module
