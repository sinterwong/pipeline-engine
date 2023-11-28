/**
 * @file test_face_keypoints.cc
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-11-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "infer/faceKeyPoints.hpp"
#include "logger/logger.hpp"
#include "visionInfer.hpp"
#include <gflags/gflags.h>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace infer;

using namespace common;

const auto initLogger = []() -> decltype(auto) {
  FlowEngineLoggerInit(true, true, true, true);
  return true;
}();

DEFINE_string(img, "", "Specify a image which contains some face path.");

constexpr int INPUT_SIZE = 192;

using algo_ptr = std::shared_ptr<AlgoInfer>;

algo_ptr getVision(AlgoConfig &&config) {

  std::shared_ptr<AlgoInfer> vision = std::make_shared<VisionInfer>(config);
  if (!vision->init()) {
    FLOWENGINE_LOGGER_ERROR("Failed to init vision");
    std::exit(-1); // 强制中断
    return nullptr;
  }
  return vision;
}

void inference(cv::Mat &image, InferResult &ret,
               std::shared_ptr<AlgoInfer> vision) {

  RetBox region{"hello"};

  InferParams params{std::string("hello"),
                     ColorType::NV12,
                     0.0,
                     region,
                     {image.cols, image.rows, image.channels()}};

  // 制作输入数据
  FrameInfo frame;
  frame.shape = {image.cols, image.rows * 2 / 3, 3};
  frame.inputShape = {640, 640, 3};
  frame.type = params.frameType;
  frame.data = reinterpret_cast<void **>(&image.data);
  vision->infer(frame, params, ret);
}

cv::Mat transform(int size, cv::Point2f center, double scale, double rotation) {
  // Step 1: 缩放变换
  cv::Mat scaleMat = cv::getRotationMatrix2D(center, 0, scale);
  cv::Mat scaleMat3x3 = cv::Mat::eye(3, 3, scaleMat.type());
  scaleMat.copyTo(scaleMat3x3.rowRange(0, 2));

  // Step 2: 向原始中心的负方向平移
  cv::Mat transToNegCenterMat = cv::Mat::eye(3, 3, scaleMat.type());
  transToNegCenterMat.at<double>(0, 2) = -center.x;
  transToNegCenterMat.at<double>(1, 2) = -center.y;

  // Step 3: 旋转变换
  cv::Mat rotMat = cv::getRotationMatrix2D(cv::Point2f(0, 0), rotation, 1);
  cv::Mat rotMat3x3 = cv::Mat::eye(3, 3, rotMat.type());
  rotMat.copyTo(rotMat3x3.rowRange(0, 2));

  // Step 4: 向图像中心的平移
  cv::Mat transToImgCenterMat = cv::Mat::eye(3, 3, rotMat.type());
  transToImgCenterMat.at<double>(0, 2) = size / 2.0;
  transToImgCenterMat.at<double>(1, 2) = size / 2.0;

  // 组合所有变换
  cv::Mat combinedMat3x3 =
      transToImgCenterMat * rotMat3x3 * transToNegCenterMat * scaleMat3x3;

  // 将3x3矩阵转换回2x3矩阵
  cv::Mat combinedMat = combinedMat3x3.rowRange(0, 2);

  combinedMat.convertTo(combinedMat, CV_32FC1);

  return combinedMat;
}

cv::Mat normCrop(cv::Mat const &image, cv::Mat const &rotMat, int size) {
  cv::Mat aligned_face;
  cv::warpAffine(image, aligned_face, rotMat, cv::Size(size, size));

  return aligned_face;
}

int main(int argc, char **argv) {

  gflags::SetUsageMessage("Face recognition");
  gflags::SetVersionString("1.0.0");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  PointsDetAlgo faceDet_config{{
                                   1,
                                   {"input"},
                                   {"output"},
                                   "/opt/deploy/models/yolov5n-face-sim.engine",
                                   "YoloPDet",
                                   {640, 640, 3},
                                   false,
                                   255.0,
                                   0,
                                   0.3,
                               },
                               5,
                               0.4};
  AlgoConfig fdet_config;
  fdet_config.setParams(faceDet_config);

  PointsDetAlgo facePoints_config{
      {
          1,
          {"input"},
          {"output"},
          "/opt/deploy/models/2d106det_192x192_nv12.bin",
          "FaceKeyPoints",
          {INPUT_SIZE, INPUT_SIZE, 3},
          false,
          255.0,
          0,
          0.3,
      },
      106,
      0.4};

  AlgoConfig fpoints_config;
  fpoints_config.setParams(facePoints_config);

  auto faceDet = getVision(std::move(fdet_config));
  auto facePoints = getVision(std::move(fpoints_config));

  // 图片读取
  cv::Mat image_bgr = cv::imread(FLAGS_img);

  cv::Mat image_rgb;
  cv::cvtColor(image_bgr, image_rgb, cv::COLOR_BGR2RGB);

  cv::Mat image_nv12;
  infer::utils::RGB2NV12(image_rgb, image_nv12);

  InferResult faceDetRet;
  inference(image_nv12, faceDetRet, faceDet);
  auto kbboxes = std::get_if<KeypointsBoxes>(&faceDetRet.aRet);
  if (!kbboxes) {
    FLOWENGINE_LOGGER_ERROR("Not a single face was detected!");
    return -1;
  }

  for (auto &kbbox : *kbboxes) {
    cv::Mat ori_face_nv12, ori_face_rgb;
    cv::Rect2i rect{static_cast<int>(kbbox.bbox.bbox[0]),
                    static_cast<int>(kbbox.bbox.bbox[1]),
                    static_cast<int>(kbbox.bbox.bbox[2] - kbbox.bbox.bbox[0]),
                    static_cast<int>(kbbox.bbox.bbox[3] - kbbox.bbox.bbox[1])};
    // 可视化人脸检测结果：人脸框和5个关键点
    cv::rectangle(image_bgr, rect, cv::Scalar(0, 0, 255), 2);
    infer::utils::cropImage(image_nv12, ori_face_nv12, rect, ColorType::NV12);
    int i = 1;
    std::vector<cv::Point2f> points;
    for (auto &p : kbbox.points) {
      cv::circle(image_bgr,
                 cv::Point{static_cast<int>(p.x), static_cast<int>(p.y)}, 3,
                 cv::Scalar{255, 255, 0});
      cv::putText(image_bgr, std::to_string(i++),
                  cv::Point(static_cast<int>(p.x), static_cast<int>(p.y)),
                  cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 14, 50), 1);
      points.push_back(cv::Point2f{p.x, p.y});
    }

    // TODO 基于5个点的人脸关键点矫正
    std::string prefix = std::to_string(rand() % 1000);
    utils::NV12toRGB(ori_face_nv12, ori_face_rgb);
    cv::imwrite(prefix + "_ori_face.jpg", ori_face_rgb);

    // 计算仿射变换的两个部分：旋转和缩放
    float w = kbbox.bbox.bbox[2] - kbbox.bbox.bbox[0];
    float h = kbbox.bbox.bbox[3] - kbbox.bbox.bbox[1];
    cv::Point2f center =
        cv::Point2f{kbbox.bbox.bbox[0] + (w / 2), kbbox.bbox.bbox[1] + (h / 2)};
    // 旋转矩阵
    int rotation = 0;
    float maxSize = std::max(w, h) * 1.5;
    float scale = INPUT_SIZE / maxSize;

    cv::Mat rotMat = transform(INPUT_SIZE, center, scale, rotation);
    std::cout << "rotMat: " << rotMat << std::endl;

    cv::Mat aligned_face_rgb = normCrop(image_rgb, rotMat, INPUT_SIZE);
    cv::imwrite(prefix + "_aligned_face.jpg", aligned_face_rgb);

    cv::Mat aligned_face_nv12;
    utils::RGB2NV12(aligned_face_rgb, aligned_face_nv12);

    // TODO 人脸特征提取
    InferResult facePointsRet;
    facePointsRet.aRet =
        KeypointsRet{.points = {}, .M = reinterpret_cast<float *>(rotMat.data)};
    inference(aligned_face_nv12, facePointsRet, facePoints);
    auto ret = std::get_if<KeypointsRet>(&facePointsRet.aRet);
    if (!ret) {
      FLOWENGINE_LOGGER_ERROR("facePointsRet is not a KeypointsBoxes");
      return -1;
    }
    
    // 可视化人脸关键点
    for (auto &p : ret->points) {
      cv::circle(image_bgr,
                 cv::Point{static_cast<int>(p.x), static_cast<int>(p.y)}, 3,
                 cv::Scalar{255, 255, 0});
    }
    cv::imwrite(prefix + "_image_bgr_face_keypoints.jpg", image_bgr);
    break;
  }

  gflags::ShutDownCommandLineFlags();
  return 0;
}