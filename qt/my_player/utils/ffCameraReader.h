#pragma once

#include <QtCore>
#include <QImage>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


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

#ifdef __cplusplus
}
#endif

//ffmpeg -devices 列出所有可用的 devices

class ffCameraReader : public QObject {

    Q_OBJECT

signals:
    void sigGotFrame(QImage);

public:
    ffCameraReader(QString path);
    ~ffCameraReader();
    int readFrames();
    int getFps();

private:
    QString mDeviceName;

    AVFormatContext *pFormatCtx;
    AVPacket *pkt;
    AVCodecContext *pCodecCtx;
    int videoIndex;
    AVFrame *pFrame;
    AVFrame *pFrame4QImage;
    struct SwsContext *img_convert_ctx;

    //int mWidth;
    //int mHeight;
    //int mFps;
};


