#pragma once

#include <QtCore>
#include <QImage>

extern "C"
{
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libswscale/swscale.h>

#include <libavutil/pixfmt.h>
#include <libavutil/imgutils.h>
}

#include <opencv2/opencv.hpp>


class YuvFileReader {

public:
    YuvFileReader(QString path);
    ~YuvFileReader();
    void setWidthHeight(int width, int height);
    QImage readFrame();

private:
    QString mPath;
    int mWidth;
    int mHeight;
    FILE *mfr;

    struct SwsContext *img_convert_ctx;
    AVFrame *frame_yuv;
    AVFrame *frame_rgb;
    uint8_t *rgb32_buffer;
};



class OpencvReader {

public:
    OpencvReader(QString path);
    ~OpencvReader();
    QImage readFrame();

private:
    cv::VideoCapture videoCap;
    cv::Mat mImage;

    QString mPath;
    int mWidth;
    int mHeight;
    int mFps;
};


