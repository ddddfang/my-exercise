#include "YuvFileReader.h"

//#include <stdio.h>
//#include <stdlib.h>
//#include <stdint.h>
#include <iostream>


/* 使用
func()
    YuvFileReader yuv_reader(mFilePath);
    yuv_reader.setWidthHeight(640, 360);
    while()
        QImage img = yuv_reader.readFrame();
*/


YuvFileReader::YuvFileReader(QString path)
    : mPath(path), mWidth(0), mHeight(0) {

    std::cout << "construct YuvFileReader, file=" << mPath.toStdString() << std::endl;
    this->mfr = fopen(mPath.toStdString().c_str(), "rb");
    this->img_convert_ctx = NULL;
    this->frame_yuv = NULL;
    this->frame_rgb = NULL;
}

YuvFileReader::~YuvFileReader() {
    std::cout << "destruct YuvFileReader" << std::endl;
    if (this->img_convert_ctx) {
        sws_freeContext(this->img_convert_ctx);
    }
    if (this->rgb32_buffer) {
        av_free(this->rgb32_buffer);
    }
    if (this->frame_yuv) {
        av_frame_free(&this->frame_yuv);
    }
    if (this->frame_rgb) {
        av_frame_free(&this->frame_rgb);
    }
    if (this->mfr) {
        fclose (this->mfr);
        this->mfr = nullptr;
    }
}

void YuvFileReader::setWidthHeight(int width, int height) {
    mWidth = width;
    mHeight = height;

    if (mWidth > 0 && mHeight > 0) {
        this->frame_yuv = av_frame_alloc();
        this->frame_rgb = av_frame_alloc();

        //int rgb32_size = avpicture_get_size(AV_PIX_FMT_RGB32, mWidth, mHeight);
        int rgb32_size = av_image_get_buffer_size(AV_PIX_FMT_RGB32, mWidth, mHeight, 1);
        this->rgb32_buffer = (uint8_t *)av_malloc(rgb32_size);

        //将 uint8_t *buffer 与 AVFrame 绑定
        //avpicture_fill((AVPicture *)this->frame_rgb, this->rgb32_buffer, AV_PIX_FMT_RGB32, mWidth, mHeight);
        av_image_fill_arrays(   this->frame_rgb->data, this->frame_rgb->linesize,
                                this->rgb32_buffer,
                                AV_PIX_FMT_RGB32, mWidth, mHeight, 1);

        this->img_convert_ctx = sws_getContext( mWidth, mHeight, AV_PIX_FMT_YUV420P,    //src
                                                mWidth, mHeight, AV_PIX_FMT_RGB32,      //dst
                                                SWS_BICUBIC, NULL, NULL, NULL);
    }
}

int YuvFileReader::readFrames() {
    int ret = 0;
    frame_yuv->format = AV_PIX_FMT_YUV420P;
    frame_yuv->width = mWidth;
    frame_yuv->height = mHeight;
    ret = av_frame_get_buffer(frame_yuv, 32);   //32字节对齐, data[] linesize[] validate!
    if (ret < 0) {
        std::cout << "error when av_frame_get_buffer." << std::endl;
        return -1;
    }
    ret = av_frame_make_writable(frame_yuv);
    if (ret < 0) {
        std::cout << "error when av_frame_make_writable." << std::endl;
        return -1;
    }
    frame_yuv->pts = -1;

    int luma_size = mWidth * mHeight;   //mWidth=640, mHeight=360, frame_yuv->linesize[0]=640,frame_yuv->linesize[1]=320
    int chroma_size = luma_size / 4;
    //std::cout << "("<<frame_yuv->linesize[0]<<","<<frame_yuv->linesize[1]<<","<<frame_yuv->linesize[2]<<").luma_size " <<luma_size<<" chroma_size "<<chroma_size<< std::endl;
    if( fread( frame_yuv->data[0], 1, luma_size, this->mfr) != (unsigned)luma_size ) {
        std::cout << "error when fread." << std::endl;
        return -1;
    }
    if( fread( frame_yuv->data[1], 1, chroma_size, this->mfr) != (unsigned)chroma_size ) {
        std::cout << "error when fread." << std::endl;
        return -1;
    }
    if( fread( frame_yuv->data[2], 1, chroma_size, this->mfr) != (unsigned)chroma_size ) {
        std::cout << "error when fread." << std::endl;
        return -1;
    }
    
    // Convert the image from its native format to RGB
    sws_scale(  this->img_convert_ctx,
                (uint8_t const * const *)frame_yuv->data, frame_yuv->linesize,
                0, mHeight,
                frame_rgb->data, frame_rgb->linesize);
    //send the image to main thread
    QImage img((uchar *)rgb32_buffer, mWidth, mHeight, QImage::Format_RGB32);
    emit sigGotFrame(img.copy());
    return 0;
}

int YuvFileReader::getFps() {
    int t = 25;  //40ms
    return t;
}

