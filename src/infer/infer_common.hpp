/**
 * @file infer_common.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-04
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "common/common.hpp"
#include <array>
#include <vector>

#ifndef __INFERENCE_COMMON_H_
#define __INFERENCE_COMMON_H_

namespace infer {

struct alignas(float) DetectionResult {
  // x y w h
  std::array<float, 4> bbox; // [x1, y1, x2, y2]
  float det_confidence;
  float class_id;
  float class_confidence;
};

struct Result {
  std::vector<DetectionResult> detResults;
  std::pair<int, float> classResult;
  std::array<int, 3> shape;
};

// 模型装载后可以获取模型的基础信息供外界使用
struct ModelInfo{
  int output_count;  // 输出的个数
  std::vector<std::vector<int>> outputShapes;  // 输出的尺度
};

} // namespace infer

#endif