#include "videoThread.h"
#include <iostream>
#include <QFile>


videoThread::videoThread()
    : mStop(false), mPause(false), mFilePath("") {
    std::cout << "videoThread constructed: threadid = " << QThread::currentThreadId() << std::endl;
}

videoThread::~videoThread() {
    std::cout << "videoThread decstructed: threadid = " << QThread::currentThreadId() << std::endl;
}

void videoThread::myStop() {
    std::cout <<"videoThread::stop call. id=" << currentThreadId() << std::endl;
    mMutex.lock();
    mStop = true;
    mMutex.unlock();
}

void videoThread::myPause(bool bpause) {
    std::cout <<"videoThread::pause call. id=" << currentThreadId() << std::endl;
    mMutex.lock();
    mPause = bpause;
    mMutex.unlock();
}

bool videoThread::isPaused() {
    bool result = false;
    mMutex.lock();
    result = mPause;
    mMutex.unlock();
    return result;
}

void videoThread::setFilePath(QString path) {
    mFilePath = path;
    QFile file(mFilePath);
    if (!file.exists()) {    //
        std::cout << "file not exist : " << mFilePath.toStdString() << std::endl;
    }
    std::cout << "path set : " << mFilePath.toStdString() << std::endl;
}

void videoThread::run() {
    std::cout << "videoThread run: threadid = " << QThread::currentThreadId() << std::endl;
    int fps = 25;

    //YuvFileReader yuv_reader(mFilePath);
    //connect(&yuv_reader, SIGNAL(sigGotFrame(QImage)), this, SLOT(onGotFrame(QImage)));
    //yuv_reader.setWidthHeight(640, 360);
    //fps = yuv_reader.getFps();

    //OpencvReader cv_reader(mFilePath);
    //connect(&cv_reader, SIGNAL(sigGotFrame(QImage)), this, SLOT(onGotFrame(QImage)));
    //fps = cv_reader.getFps();

    ffCameraReader ff_reader(mFilePath);
    connect(&ff_reader, SIGNAL(sigGotFrame(QImage)), this, SLOT(onGotFrame(QImage)));
    fps = ff_reader.getFps();

    while (true) {
        mMutex.lock();
        if(mStop) {
            mMutex.unlock();
            break;
        } else if (mPause) {
            mMutex.unlock();
            QThread::msleep(10);
            continue;
        }
        mMutex.unlock();

        //std::cout << "videoThread runing: threadid = " << QThread::currentThreadId() << std::endl;


        //readFrames 将会 emit 信号,由本thread的 onGotFrame 接收
        //if (yuv_reader.readFrames() == -1) {
        //    std::cout << "encounter error or eof." << std::endl;
        //    this->stop();
        //}
        //if (cv_reader.readFrames() == -1) {
        //    std::cout << "encounter error or eof." << std::endl;
        //    this->stop();
        //}
        if (ff_reader.readFrames() == -1) {
            std::cout << "encounter error or eof." << std::endl;
            this->myStop();
        }


        QThread::msleep(1000/fps);
    }
    //flush here
    //
    std::cout << "videoThread exit: threadid = " << QThread::currentThreadId() << std::endl;
    return;
}

//此回调函数 就是吧拿到的img再发送出去,myWidget只和thread接触,不和底下接触
void videoThread::onGotFrame(QImage img) {
    if (img.size().width() <= 0) {
        std::cout << "reach the file end." << std::endl;
        this->myStop();
        return;
    }
    //std::cout << "video thread got a frame, and here emit it" << std::endl;
    emit sigGotFrame(img);
}

