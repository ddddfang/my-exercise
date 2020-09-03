#pragma once

#include <QtCore>
#include <QImage>

#include <opencv2/opencv.hpp>


class OpencvReader {

public:
    OpencvReader(QString path);
    ~OpencvReader();
    QImage readFrame();

private:
    cv::VideoCapture videoCap;
    cv::Mat mImage;

    QString mPath;
    int mWidth;
    int mHeight;
    int mFps;
};


