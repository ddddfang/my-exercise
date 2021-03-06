#pragma once

#include <QThread>
#include <QMutex>
#include <QImage>

//#include "utils/YuvFileReader.h"
//#include "utils/OpencvReader.h"
//#include "utils/ffCameraReader.h"
#include "common.h"

class demuxThread : public QThread {

    Q_OBJECT

signals:
    void sigGotVideoFrame(QImage);
    void sigGotAudioFrame(AFrame);

private slots:
    void onGotVideoFrame(QImage);
    void onGotAudioFrame(AFrame);

public:
    explicit demuxThread();
    ~demuxThread();
    void myStop();
    void myPause(bool);
    bool isPaused();
    void setFilePath(QString path);

protected:
    void run(); //这是 QThread 定义的方法

private:
    bool mStop;     //
    bool mPause;     //
    QMutex mMutex;  //
    QString mFilePath;   //local file path
};


