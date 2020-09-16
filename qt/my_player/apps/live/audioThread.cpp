#include "audioThread.h"
#include <iostream>
#include <QFile>


audioThread::audioThread()
    : mStop(false), mPause(false), mFilePath("") {
    std::cout << "audioThread constructed: threadid = " << QThread::currentThreadId() << std::endl;
}

audioThread::~audioThread() {
    std::cout << "audioThread decstructed: threadid = " << QThread::currentThreadId() << std::endl;
}

void audioThread::myStop() {
    std::cout <<"audioThread::stop call. id=" << currentThreadId() << std::endl;
    mMutex.lock();
    mStop = true;
    mMutex.unlock();
}

void audioThread::myPause(bool bpause) {
    std::cout <<"audioThread::pause call. id=" << currentThreadId() << std::endl;
    mMutex.lock();
    mPause = bpause;
    mMutex.unlock();
}

bool audioThread::isPaused() {
    bool result = false;
    mMutex.lock();
    result = mPause;
    mMutex.unlock();
    return result;
}

void audioThread::setFilePath(QString path) {
    mFilePath = path;
    QFile file(mFilePath);
    if (!file.exists()) {    //
        std::cout << "file not exist : " << mFilePath.toStdString() << std::endl;
    }
    std::cout << "path set : " << mFilePath.toStdString() << std::endl;
}

void audioThread::run() {
    std::cout << "audioThread run: threadid = " << QThread::currentThreadId() << std::endl;

    ffMicroReader ff_reader("");
    connect(&ff_reader, SIGNAL(sigGotFrame(AFrame)), this, SLOT(onGotFrame(AFrame)));

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

        //std::cout << "audioThread runing: threadid = " << QThread::currentThreadId() << std::endl;


        //readFrames 将会 emit 信号,由本thread的 onGotFrame 接收
        if (ff_reader.readFrames() == -1) {
            std::cout << "encounter error or eof." << std::endl;
            this->myStop();
        }


        QThread::usleep(400);
    }
    //flush here
    //
    std::cout << "audioThread exit: threadid = " << QThread::currentThreadId() << std::endl;
    return;
}

//此回调函数 就是吧拿到的img再发送出去,myWidget只和thread接触,不和底下接触
void audioThread::onGotFrame(AFrame af) {
    if (af.len <= 0) {
        std::cout << "reach the file end." << std::endl;
        this->myStop();
        return;
    }
    emit sigGotFrame(af);
}

