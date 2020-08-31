#pragma once

#include <QThread>
#include <QMutex>
#include <QImage>

//#include <iostream>
//#include <stdio.h>
//extern "C"
//{
//#include <libavcodec/avcodec.h>
//#include <libavformat/avformat.h>
//#include <libavfilter/avfilter.h>
//#include <libavutil/pixfmt.h>
//#include <libswscale/swscale.h>
//}


class readerThread : public QThread {

    Q_OBJECT

public:
    explicit readerThread();
    ~readerThread();
    void stop();
    void setFilePath(QString path);

protected:
    void run(); //这是 QThread 定义的方法

signals:
    void sigNum(int);

private:
    bool mStop;     //
    QMutex mMutex;  //
    QString mFilePath;   //local file path
};


