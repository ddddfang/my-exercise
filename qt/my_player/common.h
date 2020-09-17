
#pragma once

#include <iostream>


#define MAX_AUDIO_FRAME_SIZE    192000
typedef struct AFrame {
    uint8_t data[MAX_AUDIO_FRAME_SIZE];
    int len;
} AFrame;



//qt 播放 pcm
class MyIODevice : public QIODevice {
public:
    MyIODevice(QList<AFrame> *audioFrames, QObject *parent = nullptr) : QIODevice(parent) {
        pFrames = audioFrames;
    }
    ~MyIODevice() { }

    virtual qint64 readData(char *data, qint64 maxlen) {
        qint64 readlen = 0;
        qint64 reslen = maxlen;

        while(!pFrames->isEmpty()) {
            if ((*pFrames)[0].len > reslen) {
                memcpy(data, (*pFrames)[0].data, reslen);
                (*pFrames)[0].len -= reslen;
                readlen += reslen;
                return readlen;
            } else {
                AFrame af = pFrames->takeFirst();
                memcpy(data, af.data, af.len);
                reslen -= af.len;
                readlen += af.len;
            }
        }
        std::cout << "in myIODevice::readData. read "<<readlen<<" bytes." << std::endl;
        return readlen;
    }

    virtual qint64 writeData(const char *data, qint64 len) {
        Q_UNUSED(data);
        Q_UNUSED(len);
        return 0;
    }

private:
    QList<AFrame> *pFrames;
};


