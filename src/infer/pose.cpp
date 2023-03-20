/**
 * @file pose.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-12-22
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "pose.hpp"
#include <algorithm>
#include <array>
#include <vector>

namespace infer {
namespace vision {

bool Pose::processInput(cv::Mat const &input, void **output,
                        common::ColorType) const {
  // 后面可以根据需求，实现基于opencv的预处理，比如resize和图片类型转换（bgr->rgb,
  // bgr->nv12, nv12->bgr..)
  return true;
}

bool Pose::processOutput(void **output, InferResult &result) const {
  auto poseRet = Points2f();
  generatePoints(poseRet, output);
  result.aRet = std::move(poseRet);
  return true;
}

bool Pose::verifyOutput(InferResult const &) const { return true; }
} // namespace vision
} // namespace infer