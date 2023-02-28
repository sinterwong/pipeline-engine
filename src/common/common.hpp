/**
 * @file common.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-04-23
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef _FLOWENGINE_COMMON_COMMON_HPP_
#define _FLOWENGINE_COMMON_COMMON_HPP_

#include "algo_header.hpp"
#include "infer_header.hpp"
#include "module_header.hpp"

namespace common {
/**
 * @brief no copy type
 *
 */
struct NonCopyable {
  NonCopyable() = default;
  NonCopyable(const NonCopyable &) = delete;
  NonCopyable &operator=(const NonCopyable &) = delete;
};

} // namespace common
#endif
