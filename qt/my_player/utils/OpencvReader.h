#pragma once

#include <QtCore>
#include <QImage>

#include <opencv2/opencv.hpp>


class OpencvReader : public QObject {

    Q_OBJECT

signals:
    void sigGotFrame(QImage);

public:
    OpencvReader(QString path);
    ~OpencvReader();
    int readFrames();
    int getFps();

private:
    cv::VideoCapture videoCap;
    cv::Mat mImage;

    QString mPath;
    int mWidth;
    int mHeight;
    int mFps;
    int dev_index;
};


