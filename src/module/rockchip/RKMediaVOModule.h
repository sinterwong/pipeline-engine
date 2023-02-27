#ifndef METAENGINE_RKMEDIARTSPMODULE_H
#define METAENGINE_RKMEDIARTSPMODULE_H

#include <easymedia/rkmedia_api.h>
#include <rga/im2d.h>
#include <rga/rga.h>

#include <assert.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <iostream>

#include "sample_common.h"
#include "rtsp_demo.h"
#include "rkmedia_api.h"
#include "rkmedia_venc.h"

#include "module.hpp"
#include "frameMessage.pb.h"

class RKMediaVOModule : public Module
{
protected:
    // basic
    int videoHeight, videoWidth;
    int screenHeight, screenWidth;
    bool initSuccess;

    // rga
    RGA_ATTR_S stRgaAttr;

    // VO
    VO_CHN_ATTR_S stVoAttr;
    float fltImgRatio;

    // buffer
    MB_POOL_PARAM_S stBufferPoolParam;
    MEDIA_BUFFER_POOL mbp;

    // bind and connect
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    MPP_CHN_S stEncChn;

public:
    RKMediaVOModule(backend_ptr ptr,
                    const std::string &streamName,
                    std::tuple<int, int> videoSize,
                    std::tuple<int, int> screenSize,
                    std::string const &name,
                    std::string const &type,
                    
                         );

    ~RKMediaVOModule();

    void
    forward(std::vector<forwardMessage> &message) override;
};


#endif //METAENGINE_RKMEDIARTSPMODULE_H
