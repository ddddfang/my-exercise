#include "readerThread.h"
#include <iostream>
#include <QFile>


readerThread::readerThread()
    : mStop(false), mPause(false), mFilePath("") {
    std::cout << "readerThread constructed: threadid = " << QThread::currentThreadId() << std::endl;
}

readerThread::~readerThread() {
    std::cout << "readerThread decstructed: threadid = " << QThread::currentThreadId() << std::endl;
}

void readerThread::stop() {
    std::cout <<"readerThread::stop call. id=" << currentThreadId() << std::endl;
    mMutex.lock();
    mStop = true;
    mMutex.unlock();
}

void readerThread::pause(bool bpause) {
    std::cout <<"readerThread::pause call. id=" << currentThreadId() << std::endl;
    mMutex.lock();
    mPause = bpause;
    mMutex.unlock();
}

void readerThread::setFilePath(QString path) {
    mFilePath = path;
    QFile file(mFilePath);
    if (!file.exists()) {    //
        std::cout << "file not exist : " << mFilePath.toStdString() << std::endl;
    }
    std::cout << "path set : " << mFilePath.toStdString() << std::endl;
}

void readerThread::run() {
    std::cout << "readerThread run: threadid = " << QThread::currentThreadId() << std::endl;
    int fps = 25;

    //YuvFileReader yuv_reader(mFilePath);
    //yuv_reader.setWidthHeight(640, 360);

    //OpencvReader cv_reader(mFilePath);
    //fps = 30;

    ffCameraReader ff_reader(mFilePath);

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

        //std::cout << "readerThread runing: threadid = " << QThread::currentThreadId() << std::endl;


        //QImage img = yuv_reader.readFrame();
        //QImage img = cv_reader.readFrame();
        QImage img = ff_reader.readFrame();


        //if (img.size().width() <= 0) {
        //    std::cout << "reach the file end." << std::endl;
        //    break;
        //}
        //emit sigGotFrame(img);
        QThread::msleep(1000/fps);
    }
    std::cout << "readerThread exit: threadid = " << QThread::currentThreadId() << std::endl;
    return;
}

