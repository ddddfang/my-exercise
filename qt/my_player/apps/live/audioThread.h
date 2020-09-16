#pragma once

#include <QThread>
#include <QMutex>
#include <QImage>

//#include "utils/YuvFileReader.h"
//#include "utils/OpencvReader.h"
//#include "utils/ffCameraReader.h"

#include "utils/ffMicroReader.h"

class audioThread : public QThread {

    Q_OBJECT

signals:
    void sigGotFrame(AFrame);

private slots:
    void onGotFrame(AFrame);

public:
    explicit audioThread();
    ~audioThread();
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


