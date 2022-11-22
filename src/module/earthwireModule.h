/**
 * @file earthwireModule.h
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-08-30
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __METAENGINE_EARTHWIRE_MODULE_H_
#define __METAENGINE_EARTHWIRE_MODULE_H_

#include <any>
#include <memory>
#include <vector>

#include "common/common.hpp"
#include "logger/logger.hpp"
#include "logicModule.h"

namespace module {
class EarthwireModule : public LogicModule {

public:
  EarthwireModule(Backend *ptr, const std::string &initName,
                  const std::string &initType,
                  const common::LogicConfig &logicConfig);
  ~EarthwireModule() {}

  virtual void forward(std::vector<forwardMessage> &message) override;
};
} // namespace module
#endif // __METAENGINE_EARTHWIRE_MODULE_H_
