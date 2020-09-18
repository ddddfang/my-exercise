#include "encoder.h"

//#include <stdio.h>
//#include <stdlib.h>
//#include <stdint.h>
#include <iostream>


VideoEncoder::VideoEncoder()
    : mWidth(0), mHeight(0) {
}

VideoEncoder::~VideoEncoder() {
    x264_encoder_close( this->h );
    x264_picture_clean( &(this->pic) );
}

void VideoEncoder::setWidthHeight(int width, int height) {
    mWidth = width;
    mHeight = height;

    if (mWidth > 0 && mHeight > 0) {
        /* Get default params for preset/tuning */
        if( x264_param_default_preset( &(this->param), "medium", NULL ) < 0 ) {
            std::cout << "x264_param_default_preset fail." << std::endl;
        }

        /* Configure non-default params */
        this->param.i_log_level = X264_LOG_DEBUG;
        this->param.i_bitdepth = 8;
        this->param.i_csp = X264_CSP_RGB;//X264_CSP_I420;
        this->param.i_width  = width;
        this->param.i_height = height;
        this->param.b_vfr_input = 0;      //b_vfr_input 指定是否使用时间戳用于帧率控制
        this->param.b_repeat_headers = 1; //b_repeat_headers 指定是否在每个关键帧前添加 SPS/PPS 信息
        this->param.b_annexb = 1;         //b_annexb 指定在每个 NAL 前添加起止码或者 NAL 大小

        /* Apply profile restrictions. */
        if( x264_param_apply_profile( &(this->param), "high" ) < 0 ) {
            std::cout << "x264_param_apply_profile fail." << std::endl;
        }

        if( x264_picture_alloc( &(this->pic), this->param.i_csp, this->param.i_width, this->param.i_height ) < 0 ) {
            std::cout << "x264_picture_alloc fail." << std::endl;
        }

        this->h = x264_encoder_open( &(this->param) );
        if( ! this->h ) {
            std::cout << "x264_encoder_open fail." << std::endl;
        }
    }
}

int VideoEncoder::encode(AVFrame *frame, AVPacket *pkt, int index) {
    int luma_size = width * height;
    int chroma_size = luma_size / 4;

    /* Encode frames */
    while ( 1 ) {
        /* Read input frame */
        if( fread( pic.img.plane[0], 1, luma_size, stdin ) != (unsigned)luma_size )
            break;
        if( fread( pic.img.plane[1], 1, chroma_size, stdin ) != (unsigned)chroma_size )
            break;
        if( fread( pic.img.plane[2], 1, chroma_size, stdin ) != (unsigned)chroma_size )
            break;

        pic.i_pts = i_frame;
        i_frame_size = x264_encoder_encode( h, &nal, &i_nal, &pic, &pic_out );
        if( i_frame_size < 0 )
            goto fail3;
        else if( i_frame_size ) {
            if( !fwrite( nal->p_payload, i_frame_size, 1, stdout ) )
                goto fail3;
        }
        i_frame++;
    }

    /* Flush delayed frames */
    while( x264_encoder_delayed_frames( h ) ) { //将编码器内缓存的数据编码完成
        i_frame_size = x264_encoder_encode( h, &nal, &i_nal, NULL, &pic_out );
        if( i_frame_size < 0 )
            goto fail3;
        else if( i_frame_size ) {
            if( !fwrite( nal->p_payload, i_frame_size, 1, stdout ) )
                goto fail3;
        }
    }

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

