/**
 * @file module_header.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-28
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "algo_header.hpp"
#include "infer_header.hpp"
#include <chrono>

#ifndef _FLOWENGINE_COMMON_MODULE_HEADER_HPP_
#define _FLOWENGINE_COMMON_MODULE_HEADER_HPP_

namespace common {

using namespace std::chrono_literals;
using svector = std::vector<std::string>;

/**
 * @brief 组件的类型
 *
 */
enum class ModuleTypes { Stream, Output, Logic };

/**
 * @brief 组件的信息
 *
 */
struct ModuleInfo {
  ModuleTypes moduleType; // 组件类型
  std::string moduleName; // 组件名称
  std::string sendName;   // 下游组件
  std::string recvName;   // 上游组件
  std::string className;  // 反射类名称
};

/**
 * @brief 流组件的基本参数
 *
 */
struct StreamBase {
  int cameraId;           // 摄像机 ID
  int width;              // 视频宽度
  int height;             // 视频高度
  std::string uri;        // 流uri (file, csi, rtsp, ...)
  std::string videoCode;  // 视频编码类型（h264, h265, ..)
  std::string flowType;   // 流协议类型（rtsp, rtmp, ..)
  std::string cameraName; // 摄像机名称(uuid)
};

/**
 * @brief 输出组件基本的参数
 *
 */
struct OutputBase {
  std::string url; // 通信的url
};

// 功能模块中需要启动使用的算法
using algo_pipelines = std::vector<std::pair<SupportedAlgo, std::string>>;

/**
 * @brief 逻辑的基本参数，logic包含报警时的配置
 *
 */
struct AlarmBase {
  std::string outputDir; // 报警内容存储路径
  int videDuration;      // 报警视频录制时长
  bool isDraw;           // 报警图像是否需要标记报警信息
  float threshold; // 报警阈值（计算规则自行在不同的功能中定义）
  algo_pipelines algoPipelines;
  int eventId;      // unknow，需要原路返回给后端
  std::string page; // unknow，需要原路返回给后端
};

/**
 * @brief 前端划定区域
 *
 */
struct AttentionArea {
  Points region; // 划定区域
};

/**
 * @brief 每次执行模块的间隔时间
 *
 */
struct InferInterval {
  std::chrono::seconds interval{3}; // 间隔时间
};

struct Base : public AttentionArea, public AlarmBase, public InferInterval {
  template <typename... Args>
  Base(Args &&...args)
      : AttentionArea(std::forward<Args>(args)...),
        AlarmBase(std::forward<Args>(args)...),
        InferInterval(std::forward<Args>(args)...) {}
};

struct WithoutHelmetMonitor : public Base {
  template <typename... Args>
  WithoutHelmetMonitor(Args &&...args) : Base(std::forward<Args>(args)...) {}
};

struct SmokingMonitor : public Base {
  template <typename... Args>
  SmokingMonitor(Args &&...args) : Base(std::forward<Args>(args)...) {}
};

struct ExtinguisherMonitor : public Base {
  template <typename... Args>
  ExtinguisherMonitor(Args &&...args) : Base(std::forward<Args>(args)...) {}
};

// 定义一个参数中心
class ModuleParameterCenter {
public:
  // 将所有参数类型存储在一个 std::variant 中
  using Params = std::variant<StreamBase, OutputBase, WithoutHelmetMonitor,
                              SmokingMonitor, ExtinguisherMonitor>;

  // 设置参数
  template <typename T> void setParams(T params) {
    params_ = std::move(params);
  }

  // 访问参数
  template <typename Func> void visitParams(Func &&func) {
    std::visit([&](auto &&params) { std::forward<Func>(func)(params); },
               params_);
  }

private:
  Params params_;
};

} // namespace common
#endif // _FLOWENGINE_COMMON_CONFIG_HPP_