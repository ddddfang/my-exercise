#pragma once

#include <QtCore>
#include <QImage>


#ifdef __cplusplus
extern "C"
{
#endif

#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libswscale/swscale.h>

#include <libavutil/pixfmt.h>
#include <libavutil/imgutils.h>

#include "x264.h"

#ifdef __cplusplus
}
#endif

class VideoEncoder {

public:
    VideoEncoder();
    ~VideoEncoder();
    void setWidthHeight(int width, int height);
    int encode(AVFrame *frame, AVPacket *pkt, int index = 0);

private:
    int mWidth;
    int mHeight;

    x264_param_t param;
    x264_picture_t pic;
    x264_picture_t pic_out;

    x264_nal_t *nal;
    x264_t *h;
};


