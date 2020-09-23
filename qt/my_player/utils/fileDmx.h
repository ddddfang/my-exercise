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

#include "common.h"

//ffmpeg -devices 列出所有可用的 devices

class fileDmx : public QObject {

    Q_OBJECT

signals:
    void sigGotVideoFrame(QImage);
    void sigGotAudioFrame(AFrame);

public:
    fileDmx(QString path);
    ~fileDmx();
    int readFrames();

private:
    QString mFileName;

    AVFormatContext *pFormatCtx;
    AVPacket *pkt;
    AVCodecContext *pVCodecCtx;
    AVCodecContext *pACodecCtx;
    int videoIndex;
    int audioIndex;
    //QList<AVPacket> aFrames;  //audio pkts
    AVFrame *pFrame;
    AVFrame *pFrame4QImage;
    struct SwsContext *img_convert_ctx;

    //int mWidth;
    //int mHeight;
    //int mFps;
    FILE *pFileYuv;
    FILE *pFilePcm;
};


