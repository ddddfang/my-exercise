#include "demuxThread.h"
#include <iostream>
#include <QFile>

#include "utils/fileDmx.h"

demuxThread::demuxThread()
    : mStop(false), mPause(false), mFilePath("") {
    std::cout << "demuxThread constructed: threadid = " << QThread::currentThreadId() << std::endl;
}

demuxThread::~demuxThread() {
    std::cout << "demuxThread decstructed: threadid = " << QThread::currentThreadId() << std::endl;
}

void demuxThread::myStop() {
    std::cout <<"demuxThread::stop call. id=" << currentThreadId() << std::endl;
    mMutex.lock();
    mStop = true;
    mMutex.unlock();
}

void demuxThread::myPause(bool bpause) {
    std::cout <<"demuxThread::pause call. id=" << currentThreadId() << std::endl;
    mMutex.lock();
    mPause = bpause;
    mMutex.unlock();
}

bool demuxThread::isPaused() {
    bool result = false;
    mMutex.lock();
    result = mPause;
    mMutex.unlock();
    return result;
}

void demuxThread::setFilePath(QString path) {
    mFilePath = path;
    QFile file(mFilePath);
    if (!file.exists()) {    //
        std::cout << "file not exist : " << mFilePath.toStdString() << std::endl;
    }
    std::cout << "path set : " << mFilePath.toStdString() << std::endl;
}

void demuxThread::run() {
    std::cout << "demuxThread run: threadid = " << QThread::currentThreadId() << std::endl;
    int fps = 25;

    //ffCameraReader ff_reader(mFilePath);
    //connect(&ff_reader, SIGNAL(sigGotFrame(QImage)), this, SLOT(onGotFrame(QImage)));
    //fps = ff_reader.getFps();
    fileDmx demux(mFilePath);
    connect(&demux, SIGNAL(sigGotVideoFrame(QImage)), this, SLOT(onGotVideoFrame(QImage)));
    connect(&demux, SIGNAL(sigGotAudioFrame(AFrame)), this, SLOT(onGotAudioFrame(AFrame)));

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

        //std::cout << "demuxThread runing: threadid = " << QThread::currentThreadId() << std::endl;


        //readFrames 将会 emit 信号,由本thread的 onGotFrame 接收
        if (demux.readFrames() == -1) {
            std::cout << "encounter error or eof." << std::endl;
            this->myStop();
        }


        QThread::msleep(1000/fps);
    }
    //flush here
    //
    std::cout << "demuxThread exit: threadid = " << QThread::currentThreadId() << std::endl;
    return;
}

//此回调函数 就是吧拿到的img再发送出去,myWidget只和thread接触,不和底下接触
void demuxThread::onGotVideoFrame(QImage img) {
    if (img.size().width() <= 0) {
        std::cout << "reach the file end." << std::endl;
        this->myStop();
        return;
    }
    std::cout << "got a video frame, and here emit it" << std::endl;
    //emit sigGotFrame(img);
}

void demuxThread::onGotAudioFrame(AFrame af) {
    std::cout << "got a audio frame, and here emit it" << std::endl;

}
