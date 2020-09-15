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
#include <libswresample/swresample.h>

#include <libavutil/pixfmt.h>
#include <libavutil/imgutils.h>

#ifdef __cplusplus
}
#endif


#define MAX_AUDIO_FRAME_SIZE    192000
typedef struct AFrame {
    uint8_t data[MAX_AUDIO_FRAME_SIZE];
    int len;
} AFrame;


//ffmpeg -devices 列出所有可用的 devices

class ffMicroReader : public QObject {

    Q_OBJECT
signals:
    void sigGotFrame(AFrame);

public:
    ffMicroReader(QString path);
    ~ffMicroReader();
    int readFrames();  //调用一次则 call 一下 av_read_frame(),至于得到多少帧,通过 callback 通知

private:
    QString mDeviceName;
    int audioindex;
    AVPacket *pkt;
    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx;
    AVFrame *pFrame;
#if OUTPUT_PCM
    FILE *pFile;
#endif
};


