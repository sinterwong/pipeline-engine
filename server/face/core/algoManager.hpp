/**
 * @file algoManager.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief 算法负载均衡管理，用来调度人脸逻辑。全局单例
 * @version 0.1
 * @date 2023-10-24
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "faceRecognition.hpp"
#include "logger/logger.hpp"
#include "thread_pool.h"
#include <condition_variable>
#include <future>
#include <mutex>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <queue>
#include <thread>
#include <vector>

#ifndef __SERVER_FACE_CORE_ALGO_MANAGER_HPP_
#define __SERVER_FACE_CORE_ALGO_MANAGER_HPP_

namespace server::face::core {

constexpr int ALGO_NUM = 2;

struct FramePackage {
  std::string cameraName;
  std::shared_ptr<cv::Mat> frame;
};

using algo_ptr = std::shared_ptr<FaceRecognition>;
class AlgoManager {
public:
  static AlgoManager &getInstance() {
    static std::once_flag onceFlag;
    std::call_once(onceFlag, [] { instance.reset(new AlgoManager()); });
    return *instance;
  }
  AlgoManager(AlgoManager const &) = delete;
  AlgoManager &operator=(AlgoManager const &) = delete;

public:
  std::future<bool> infer(FramePackage const &framePackage,
                          std::vector<float> &feature) {
    auto task = std::packaged_task<bool()>([&] {
      // 等待获取可用算法资源
      algo_ptr algo = getAvailableAlgo();

      // 使用算法资源进行推理
      bool ret = algo->forward(*framePackage.frame, feature);

      // 推理完成后标记算法为可用
      releaseAlgo(algo);
      return ret;
    });

    std::future<bool> ret = task.get_future();

    return ret; // 移交调用者等待算法结果
  }

  std::future<bool> infer(std::string const &url, std::vector<float> &feature) {
    auto task = std::packaged_task<bool()>([&] {
      // 等待获取可用算法资源
      algo_ptr algo = getAvailableAlgo();
    
      // TODO 使用算法资源进行推理
      cv::Mat image = cv::imread(url);
      bool ret = algo->forward(image, feature);

      // 推理完成后标记算法为可用
      releaseAlgo(algo);
      return ret;
    });

    std::future<bool> ret = task.get_future();

    return ret; // 移交调用者等待算法结果
  }

private:
  AlgoManager() {
    for (size_t i = 0; i < algos.size(); ++i) {
      // 初始化算法资源
      algos[i] = std::make_shared<FaceRecognition>();
      availableAlgos.push(algos[i]);
    }
  }
  ~AlgoManager() {}
  static std::unique_ptr<AlgoManager> instance;

private:
  // 算法资源，启动多个算法供调度，TODO 可能需要一个生产消费者模型
  std::vector<algo_ptr> algos{ALGO_NUM};
  std::queue<algo_ptr> availableAlgos;

  std::mutex m;
  std::condition_variable cv;

  algo_ptr getAvailableAlgo() {
    std::unique_lock<std::mutex> lock(m);
    cv.wait(lock, [&]() { return !availableAlgos.empty(); });
    algo_ptr algo = availableAlgos.front();
    availableAlgos.pop();
    return algo;
  }

  void releaseAlgo(algo_ptr algo) {
    std::lock_guard<std::mutex> lock(m);
    availableAlgos.push(algo);
    cv.notify_one();
  }
};

} // namespace server::face::core
#endif