
#include "ffCameraReader.h"
#include <iostream>


ffCameraReader::ffCameraReader(QString path) {
    //"/dev/video0"
    std::cout << "construct ffCameraReader, file=" << path.toStdString() << std::endl;
    int ret = 0;
    this->mDeviceName = path;   // "/dev/video0"

    avformat_network_init();    //? 似乎不需要
    avdevice_register_all();    //Register Device

    this->pFormatCtx = avformat_alloc_context();
    this->pkt = av_packet_alloc();

    /* 1.打开format并获取其中的stream信息 */
    AVInputFormat *ifmt = av_find_input_format("video4linux2");
    if (avformat_open_input(&(this->pFormatCtx), this->mDeviceName.toStdString().c_str(), ifmt, NULL) != 0) {
        std::cout << "Couldn't open input stream." << std::endl;
    }

    if (avformat_find_stream_info(this->pFormatCtx, NULL) < 0) {
        std::cout << "Couldn't find stream information." << std::endl;
    }

    /* 2.完成后做一个打印 */
    av_dump_format(this->pFormatCtx, 0, this->mDeviceName.toStdString().c_str(), 0);

    /* 3.查找video流 */
    this->videoIndex = -1;
    for (int i = 0; i < this->pFormatCtx->nb_streams; i++) {
        if (this->pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            this->videoIndex = i;
            break;
        }
    }
    if (this->videoIndex == -1) {
        std::cout << "Couldn't find a video stream." << std::endl;
    }

    /* 4.拿到video流中的 codec ctx 信息 */
    AVCodec *pCodec = avcodec_find_decoder(this->pFormatCtx->streams[this->videoIndex]->codecpar->codec_id);
    if (!pCodec) {
        std::cout << "Codec not found." << std::endl;
    }
    this->pCodecCtx = avcodec_alloc_context3(pCodec); //为 AVCodec 分配一个全新的 AVCodecContext
    if (!this->pCodecCtx) {
        std::cout << "avcodec_alloc_context3 fail." << std::endl;
    }
    //拷贝 原 stream 中的 codecpar 信息
    if (avcodec_parameters_to_context(this->pCodecCtx, this->pFormatCtx->streams[this->videoIndex]->codecpar) < 0) {
        std::cout << "avcodec_parameters_to_context fail." << std::endl;
    }

    if (avcodec_open2(this->pCodecCtx, pCodec, NULL) < 0) {
        std::cout << "Could not open codec." << std::endl;
    }

    this->pFrame = av_frame_alloc();     //原始 frame, 鬼知道什么格式
    this->pFrame4QImage = av_frame_alloc();  //统一转换为 yuv420p
    this->pFrame4QImage->format = AV_PIX_FMT_RGB24;//AV_PIX_FMT_YUV420P; QImage 只支持rgb格式
    this->pFrame4QImage->width = this->pCodecCtx->width;
    this->pFrame4QImage->height = this->pCodecCtx->height;
    ret = av_frame_get_buffer(this->pFrame4QImage, 32);   //32字节对齐. ffmpeg/doc/examples/muxing.c 里面就是这样分配的
    if (ret < 0) {
        fprintf(stderr, "error when av_frame_get_buffer for pFrame\n");
    }

    this->img_convert_ctx = sws_getContext( this->pCodecCtx->width, this->pCodecCtx->height, this->pCodecCtx->pix_fmt,
                                            this->pCodecCtx->width, this->pCodecCtx->height, (AVPixelFormat)this->pFrame4QImage->format,
                                            SWS_BICUBIC, NULL, NULL, NULL); 
}

ffCameraReader::~ffCameraReader() {
    std::cout << "destruct ffCameraReader" << std::endl;
    if (this->img_convert_ctx) {
        sws_freeContext(this->img_convert_ctx);
    }
    if (this->pFrame4QImage) {
        av_frame_free(&this->pFrame4QImage);
    }
    if (this->pFrame) {
        av_frame_free(&this->pFrame);
    }
    if (this->pCodecCtx) {
        avcodec_close(this->pCodecCtx);
        avcodec_free_context(&pCodecCtx);
    }
    if (this->pFormatCtx) {
        avformat_close_input(&pFormatCtx);//including avformat_free_context
    }
    if (this->pkt) {
        av_packet_free(&pkt);
    }
}

int ffCameraReader::readFrames() {
    int ret = 0;
    if (av_read_frame(this->pFormatCtx, this->pkt) >= 0) {
        if (this->pkt->stream_index == this->videoIndex) {

            //最新的decode方法
            ret = avcodec_send_packet(this->pCodecCtx, this->pkt);
            if (ret < 0) {
                std::cout << "Error sending a packet for decoding." << std::endl;
                return -1;
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(this->pCodecCtx, this->pFrame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                } else if (ret < 0) {
                    std::cout << "Error during decoding." << std::endl;
                    break;
                }
                sws_scale(  this->img_convert_ctx, (uint8_t const* const*)this->pFrame->data, this->pFrame->linesize,
                            0, pCodecCtx->height,
                            this->pFrame4QImage->data, this->pFrame4QImage->linesize);

                //下面这两种方式都可用,参考 https://www.thinbug.com/q/13088749
                //QImage img(pFrame4QImage->width, pFrame4QImage->height, QImage::Format_RGB888);
                //for(int y = 0; y < pFrame4QImage->height; y++) {
                //    memcpy(img.scanLine(y), this->pFrame4QImage->data[0] + y * this->pFrame4QImage->linesize[0], this->pFrame4QImage->width * 3);
                //}
                //emit sigGotFrame(img.copy());

                QImage img(this->pFrame4QImage->width, this->pFrame4QImage->height, QImage::Format_RGB32);
                uint8_t *src = (uint8_t *)(this->pFrame4QImage->data[0]);
                for (int y = 0; y < pFrame4QImage->height; y++) {
                    QRgb *scanLine = (QRgb *) img.scanLine(y);
                    for (int x = 0; x < pFrame4QImage->width; x++) {
                        scanLine[x] = qRgb(src[3*x], src[3*x+1], src[3*x+2]);
                    }
                    src += pFrame4QImage->linesize[0];  //看起来 linesize[0] 是图像宽度,的3倍啊
                }
                emit sigGotFrame(img.copy());
            }
        }
    }
    return 0;
}

int ffCameraReader::getFps() {
    int t = 25;  //40ms
    return t;
}

