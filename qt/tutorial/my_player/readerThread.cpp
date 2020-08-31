#include "readerThread.h"
#include <iostream>

readerThread::readerThread() : mStop(false), mFilePath("") {
    std::cout << "readerThread constructed: threadid = " << QThread::currentThreadId() << std::endl;
}

readerThread::~readerThread() {
    std::cout << "readerThread decstructed: threadid = " << QThread::currentThreadId() << std::endl;
}

void readerThread::stop() {
    std::cout <<"readerThread::stop call. id=" << currentThreadId();
    mMutex.lock();
    mStop = true;
    mMutex.unlock();
}

void readerThread::setFilePath(QString path) {
    mFilePath = path;
}

void readerThread::run() {
    std::cout << "readerThread run: threadid = " << QThread::currentThreadId() << std::endl;
    int i = 0;

    mMutex.lock();
    mStop = false;
    mMutex.unlock();

    while(true) {
        mMutex.lock();
        if(mStop)
        {
            mMutex.unlock();
            break;
        }
        mMutex.unlock();

        std::cout << "readerThread runing: threadid = " << QThread::currentThreadId() << std::endl;
        QThread::msleep(100);
        
        i++;
        emit sigNum(i);
        //QMutex mutex;
        //QWaitCondition sleep;
        //mutex.lock();
        //sleep.wait(&mutex, 1000);
        //mutex.unlock();

        //QTime t;
        //t.start();
        //while(t.elapsed()<1000)
        //    QCoreApplication::processEvents();
    }
    std::cout << "readerThread exit: threadid = " << QThread::currentThreadId() << std::endl;
    return;
}

