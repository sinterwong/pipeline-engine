/**
 * @file videoRecord.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-21
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "videoRecord.hpp"
#include "logger/logger.hpp"
#include <algorithm>
#include <cassert>
#include <memory>

namespace module {
namespace utils {

bool VideoRecord::init() {
  stream = XEncoder::create(params);
  if (!stream) {
    return false;
  }
  return true;
}

bool VideoRecord::check() const noexcept {
  return stream && stream->isStreaming();
}

void VideoRecord::destory() noexcept {
  if (check()) {
    stream->close();
  }
  stream = nullptr;
}

bool VideoRecord::record(void *frame) {
  stream->render(&frame);
  return true;
}
} // namespace utils
} // namespace module