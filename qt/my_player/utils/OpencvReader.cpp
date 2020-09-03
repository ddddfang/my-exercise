#include "OpencvReader.h"

#include <iostream>


OpencvReader::OpencvReader(QString path) {
    this->mPath = path;
    std::cout << "construct OpencvReader, file=" << mPath.toStdString() << std::endl;

    int dev_index = -1;
    if (path == "/dev/video0") {
        dev_index = 0;
    } else if (path == "/dev/video1") {
        dev_index = 1;
    }

    if (dev_index != -1) {  //open device
        if (!this->videoCap.open(dev_index)) {
            std::cout << "error when open device " << dev_index << std::endl;
        }
    } else {                //open file, audio将被丢弃
        if (!this->videoCap.open(path.toStdString())) {
            std::cout << "error when open file " << mPath.toStdString() << std::endl;
        }
    }

    if (this->videoCap.isOpened()) {
        //open success
        this->mWidth = this->videoCap.get(CV_CAP_PROP_FRAME_WIDTH);
        this->mHeight = this->videoCap.get(CV_CAP_PROP_FRAME_HEIGHT);
        this->mImage = cv::Mat::zeros(this->mHeight, this->mWidth, CV_8UC3);  //8bit宽度,3channel
        this->mFps = this->videoCap.get(CV_CAP_PROP_FPS);
        std::cout << "open success, ("<<this->mWidth<<","<<this->mHeight<<") fps:" << this->mFps << std::endl;
    } else {
        std::cout << "open failed." << std::endl;
    }
}

OpencvReader::~OpencvReader() {
    std::cout << "destruct OpencvReader." << std::endl;
    if (this->videoCap.isOpened()) {
        this->videoCap.release();
    }
}

QImage OpencvReader::readFrame() {
    //this->videoCap >> this->mImage;    //二者等价
    //cv::Mat targetImg;
    //if (this->videoCap.read(targetImg)) {
    //    if (targetImg.data) {
    //        //cv::cvtColor(this->mImage, this->mImage, CV_YUV2RGBA_YUY2);
    //        //cv::cvtColor(this->mImage, this->mImage, CV_YUV2RGB_YUY2);   //qt support RGB
    //        cv::cvtColor(targetImg, targetImg, CV_YUV2RGB);   //qt support RGB
    //        QImage tmpImg = QImage((uint8_t *)targetImg.data, targetImg.cols, targetImg.rows, QImage::Format_RGB888);
    //        QImage ret = tmpImg.copy();
    //        return ret;
    //    }
    //}

    if (this->videoCap.read(this->mImage)) {
        if (this->mImage.data) {
            cv::cvtColor(this->mImage, this->mImage, CV_BGR2RGB);   //opencv generate bgr, qt support RGB
            QImage tmpImg = QImage((uint8_t *)this->mImage.data, this->mImage.cols, this->mImage.rows, QImage::Format_RGB888);
            QImage ret = tmpImg.copy();
            return ret;
        }
    }
    return QImage();
}
