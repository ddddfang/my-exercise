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

class YuvFileReader : public QObject {

    Q_OBJECT

signals:
    void sigGotFrame(QImage);

public:
    YuvFileReader(QString path);
    ~YuvFileReader();
    void setWidthHeight(int width, int height);
    int readFrames();
    int getFps();

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


