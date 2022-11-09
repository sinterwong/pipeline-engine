/**
 * @file StreamGenerator.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-10-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "streamGenerator.h"
#include "hb_comm_video.h"
#include "hb_common.h"
#include "hb_type.h"
#include "hb_vdec.h"
#include "hb_vp_api.h"
#include "logger/logger.hpp"
#include "messageBus.h"
#include "x3_vio_vdec.hpp"
#include "x3_vio_vp.hpp"
#include <any>
#include <array>
#include <cstddef>
#include <memory>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/opencv.hpp>
#include <sys/types.h>
#include <type_traits>

namespace module {
StreamGenerator::StreamGenerator(Backend *ptr, const std::string &initName,
                                 const std::string &initType,
                                 const common::CameraConfig &_params,
                                 const std::vector<std::string> &recv,
                                 const std::vector<std::string> &send)
    : Module(ptr, initName, initType, recv, send) {

  // 编码、解码模块初始化，整个应用中需要调用一次
  HB_VDEC_Module_Init();
  ret = x3_vp_init();
  if (ret) {
    FLOWENGINE_LOGGER_ERROR("hb_vp_init failed, ret: {}", ret);
    HB_VDEC_Module_Uninit();
    return;
  }
  FLOWENGINE_LOGGER_INFO("x3_vp_init ok!");

  ret = vdec_ChnAttr_init(&vdec_chn_info.m_chn_attr, PT_H264, 1920, 1080);
  if (ret) {
    FLOWENGINE_LOGGER_ERROR("vdec_ChnAttr_init failed, {}", ret);
  }

  // 注入参数
  vdec_chn_info.vp_param.mmz_size = _params.widthPixel * _params.heightPixel;
  vdec_chn_info.m_stream_src = _params.cameraIp;
  vdec_chn_info.m_vdec_chn_id = _params.cameraId;
  vdec_chn_info.m_chn_enable = 1;

  ret = x3_vdec_init_wrap(&vdec_chn_info);
  if (ret) {
    FLOWENGINE_LOGGER_ERROR("x3_vdec_init_wrap failed, {}", ret);
  }

  ret = x3_vdec_start(vdec_chn_info.m_vdec_chn_id);
  if (ret) {
    FLOWENGINE_LOGGER_ERROR("x3_vdec_start failed, %d", ret);
  }

  // 初始化frame包装器
  memset(&pstStream, 0, sizeof(VIDEO_STREAM_S));
  memset(&stFrameInfo, 0, sizeof(VIDEO_FRAME_S));

  // ffmpeg 拉流初始化
  vdec_chn_info.av_param.firstPacket = 1;
  vdec_chn_info.av_param.videoIndex = AV_open_stream_file(
      vdec_chn_info.m_stream_src.c_str(), &avContext, &avpacket);

  if (vdec_chn_info.av_param.videoIndex < 0) {
    FLOWENGINE_LOGGER_ERROR("AV_open_stream_file failed, ret = {}",
                            vdec_chn_info.av_param.videoIndex);
  }
  cameraResult = CameraResult{static_cast<int>(_params.widthPixel),
                              static_cast<int>(_params.heightPixel),
                              25,
                              _params.cameraId,
                              _params.videoCode,
                              _params.flowType,
                              _params.cameraIp};
}

void StreamGenerator::step() {
  message.clear();
  hash.clear();
  loop = false;

  beforeGetMessage();
  beforeForward();

  forward(message);
  afterForward();
}

void StreamGenerator::delBuffer(std::vector<std::any> &list) {
  assert(list.size() == 1);
  assert(list[0].has_value());
  assert(list[0].type() == typeid(VIDEO_FRAME_S *));
  list.clear();
}

std::any getFrameInfo(std::vector<std::any> &list, FrameBuf *buf) {
  assert(list.size() == 1);
  assert(list[0].has_value());
  assert(list[0].type() == typeid(VIDEO_FRAME_S *));
  return reinterpret_cast<void *>(std::any_cast<VIDEO_FRAME_S *>(list[0]));
}

std::any getMatBuffer(std::vector<std::any> &list, FrameBuf *buf) {
  assert(list.size() == 1);
  assert(list[0].has_value());
  assert(list[0].type() == typeid(VIDEO_FRAME_S *));
  auto frameInfo = std::any_cast<VIDEO_FRAME_S *>(list[0]);
  // 设置初始化即将放入opencv的数据
  // size_t const size = buf->height * buf->width;
  // std::vector<u_char> data(size + size / 2, 0);
  // std::cout << static_cast<int>(frameInfo->stVFrame.vir_ptr[0][800]) <<
  // std::endl; data.insert(data.end(), frameInfo->stVFrame.vir_ptr[0],
  // frameInfo->stVFrame.vir_ptr[0] + size); data.insert(data.end(),
  // frameInfo->stVFrame.vir_ptr[1], frameInfo->stVFrame.vir_ptr[1] + (size /
  // 2));
  cv::Mat picYV12 =
      cv::Mat(frameInfo->stVFrame.height * 3 / 2, frameInfo->stVFrame.width,
              CV_8UC1, frameInfo->stVFrame.vir_ptr[0]);
  cv::Mat picRGB;
  cv::cvtColor(picYV12, picRGB, CV_YUV2RGB_NV12);
  // cv::imwrite("test.jpg", picRGB); // only for test
  // FLOWENGINE_LOGGER_INFO("Saved the test image");

  std::shared_ptr<cv::Mat> mat = std::make_shared<cv::Mat>(std::move(picRGB));
  // cv::cvtColor(*mat, *mat, cv::COLOR_BGR2RGB);
  return mat;
}

std::any getPtrBuffer(std::vector<std::any> &list, FrameBuf *buf) {
  assert(list.size() == 1);
  assert(list[0].has_value());
  assert(list[0].type() == typeid(VIDEO_FRAME_S *));
  auto frameInfo = std::any_cast<VIDEO_FRAME_S *>(list[0]);
  return reinterpret_cast<void **>(frameInfo->stVFrame.vir_ptr);
}

FrameBuf makeFrameBuf(VIDEO_FRAME_S *frameInfo, int height, int width) {
  FrameBuf temp;
  temp.write({std::make_any<VIDEO_FRAME_S *>(frameInfo)},
             {std::make_pair("void**", getPtrBuffer),
              std::make_pair("Mat", getMatBuffer)},
             &StreamGenerator::delBuffer,
             std::make_tuple(width, height, 3, UINT8));
  return temp;
}

void StreamGenerator::forward(std::vector<forwardMessage> message) {
  for (auto &[send, type, buf] : message) {
    if (type == "ControlMessage") {
      // FLOWENGINE_LOGGER_INFO("{} JetsonSourceModule module was done!", name);
      std::cout << name << "{} StreamGenerator module was done!" << std::endl;
      stopFlag.store(true);
      return;
    }
  }

  VDEC_CHN_STATUS_S pstStatus;
  HB_VDEC_QueryStatus(vdec_chn_info.m_vdec_chn_id, &pstStatus);
  if (pstStatus.cur_input_buf_cnt >= (uint32_t)vdec_chn_info.vp_param.mmz_cnt) {
    usleep(10000);
    return;
  }
  usleep(20000);

  mmz_index = AV_read_frame(avContext, &avpacket, &vdec_chn_info.av_param,
                            &vdec_chn_info.vp_param);
  if (mmz_index == -1) {
    FLOWENGINE_LOGGER_INFO("AV_read_frame eos");
    pstStream.pstPack.phy_ptr = vdec_chn_info.vp_param.mmz_paddr[0];
    pstStream.pstPack.vir_ptr = vdec_chn_info.vp_param.mmz_vaddr[0];
    pstStream.pstPack.pts = vdec_chn_info.av_param.count;
    pstStream.pstPack.src_idx = mmz_index;
    pstStream.pstPack.size = 0;
    pstStream.pstPack.stream_end = HB_FALSE;
    error = HB_VDEC_SendStream(vdec_chn_info.m_vdec_chn_id, &pstStream, 200);
    if (error) {
      FLOWENGINE_LOGGER_ERROR("HB_VDEC_SendStream chn{} failed, ret: {}",
                             vdec_chn_info.m_vdec_chn_id, error);
      return;
    }
    if (avContext) {
      avformat_close_input(&avContext);
    }
    stopFlag.store(true);
    return;
  }
  pstStream.pstPack.phy_ptr = vdec_chn_info.vp_param.mmz_paddr[mmz_index];
  pstStream.pstPack.vir_ptr = vdec_chn_info.vp_param.mmz_vaddr[mmz_index];
  pstStream.pstPack.pts = vdec_chn_info.av_param.count;
  pstStream.pstPack.src_idx = mmz_index;
  pstStream.pstPack.size = vdec_chn_info.av_param.bufSize;
  pstStream.pstPack.stream_end = HB_FALSE;

  error = HB_VDEC_SendStream(vdec_chn_info.m_vdec_chn_id, &pstStream, 300);
  if (error == -HB_ERR_VDEC_OPERATION_NOT_ALLOWDED ||
      error == -HB_ERR_VDEC_UNKNOWN) {
    FLOWENGINE_LOGGER_ERROR("HB_VDEC_SendStream failed\n");
  }
  // 通常和上面的HB_VDEC_SendStream分开线程处理，做成生产消费者模式，这里演示就放一起了
  error = HB_VDEC_GetFrame(vdec_chn_info.m_vdec_chn_id, &stFrameInfo, 300);
  if (error) {
    FLOWENGINE_LOGGER_ERROR("HB_VDEC_GetFrame chn{} error, ret: {}",
                            vdec_chn_info.m_vdec_chn_id, error);
    return;
  } else {
    queueMessage sendMessage;
    FrameBuf frameBufMessage = makeFrameBuf(
        &stFrameInfo, stFrameInfo.stVFrame.height,
        stFrameInfo.stVFrame.width);
    int returnKey = backendPtr->pool->write(frameBufMessage);

    // // TODO test
    // std::shared_ptr<cv::Mat> image =
    //     std::any_cast<std::shared_ptr<cv::Mat>>(frameBufMessage.read("Mat"));
    // // TODO test

    sendMessage.frameType = ColorType::NV12;
    sendMessage.key = returnKey;
    sendMessage.cameraResult = cameraResult;
    sendMessage.status = 0;
    autoSend(sendMessage);
    // FLOWENGINE_LOGGER_INFO("Send the frame message!");
    // 必要步骤
    HB_VDEC_ReleaseFrame(vdec_chn_info.m_vdec_chn_id, &stFrameInfo);
  }
}
FlowEngineModuleRegister(StreamGenerator, Backend *, std::string const &,
                         std::string const &, common::CameraConfig const &,
                         std::vector<std::string> const &,
                         std::vector<std::string> const &);
} // namespace module
