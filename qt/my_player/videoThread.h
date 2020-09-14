#pragma once

#include <QThread>
#include <QMutex>
#include <QImage>

#include "utils/YuvFileReader.h"
#include "utils/OpencvReader.h"
#include "utils/ffCameraReader.h"

class videoThread : public QThread {

    Q_OBJECT

signals:
    void sigGotFrame(QImage);

private slots:
    void onGotFrame(QImage);

public:
    explicit videoThread();
    ~videoThread();
    void pause(bool);
    void stop();
    void setFilePath(QString path);

protected:
    void run(); //这是 QThread 定义的方法

private:
    bool mStop;     //
    bool mPause;     //
    QMutex mMutex;  //
    QString mFilePath;   //local file path
};


