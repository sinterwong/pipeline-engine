/**
 * @file trt_inference.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-06-01
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __TRT_INFERENCE_H_
#define __TRT_INFERENCE_H_
#include "NvInfer.h"
#include "inference.h"
#include "jetson/argsParser.h"
#include "jetson/buffers.h"
#include "jetson/common.h"
#include <NvInferRuntimeCommon.h>
#include <array>
#include <cmath>
#include <memory>

#define IMAGE_MAX_SIZE 2560 * 1440 * 3

namespace infer {
namespace trt {
class TRTInference : public Inference {
public:
  //!
  //! \brief construction
  //!
  TRTInference(const common::AlgorithmConfig &_param) : mParams(_param) {
    CHECK(cudaStreamCreate(&processStream));
    // prepare input data cache in pinned memory
    CHECK(cudaMallocHost((void **)&imageHost, IMAGE_MAX_SIZE));
    // prepare input data cache in device memory
    CHECK(cudaMalloc((void **)&imageDevice, IMAGE_MAX_SIZE));
  }

  //!
  //! \brief destruction
  //!
  ~TRTInference() {
    cudaStreamDestroy(processStream);
    CHECK(cudaFree(imageDevice));
    CHECK(cudaFreeHost(imageHost));
  }
  //!
  //! \brief initialize the network
  //!
  virtual bool initialize() override;

  //!
  //! \brief Runs the inference engine
  //!
  virtual bool infer(void *, Result &) override;
  // virtual bool infer(cv::Mat const&, Result &) override;

private:
  //!
  //! \brief Resize input
  //!
  bool resizeInput(cv::Mat &) const;

  //!
  //! \brief Reads the input and mean data, preprocesses, and stores the result
  //! in a managed buffer
  //!
  virtual bool processInput(void *, BufferManager const &,
                            std::array<int, 3> const &) const;

  //!
  //! \brief Verifies that the output is correct and prints it
  //!
  virtual bool verifyOutput(Result const &) const;

  //!
  //! \brief Postprocessing that the output is correct and prints it
  //!
  virtual bool processOutput(BufferManager const &, Result &) const;

private:
  //!< The TensorRT engine used to run the network
  std::shared_ptr<nvinfer1::ICudaEngine> mEngine{nullptr};
  //!< The TensorRT execution context
  UniquePtr<nvinfer1::IExecutionContext> context{nullptr};
  //!< The dimensions of the input to the network.
  Dims inputDims;
  //!< The dimensions of the output to the network.
  Dims outputDims;

  uint8_t *imageHost = nullptr;
  uint8_t *imageDevice = nullptr;
  // Create stream
  cudaStream_t processStream;

protected:
  common::AlgorithmConfig mParams; //!< The parameters for the sample.
};
} // namespace trt
} // namespace infer
#endif
