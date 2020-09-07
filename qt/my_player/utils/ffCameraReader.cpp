
#include "ffCameraReader.h"
#include <iostream>

static void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize,
                     char *filename)
{
    FILE *f;
    int i;

    f = fopen(filename,"w");
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}

ffCameraReader::ffCameraReader(QString path) {
    //"/dev/video0"
    std::cout << "construct ffCameraReader, file=" << path.toStdString() << std::endl;
    int ret = 0;
    this->mDeviceName = path;   // "/dev/video0"

    avformat_network_init();    //?
    avdevice_register_all();    //Register Device

    this->pFormatCtx = avformat_alloc_context();
    this->pkt = av_packet_alloc();;

    /* 打开format并获取其中的stream信息 */
    AVInputFormat *ifmt = av_find_input_format("video4linux2");
    if (avformat_open_input(&(this->pFormatCtx), this->mDeviceName.toStdString().c_str(), ifmt, NULL) != 0) {
        std::cout << "Couldn't open input stream." << std::endl;
    }

    if (avformat_find_stream_info(this->pFormatCtx, NULL) < 0) {
        std::cout << "Couldn't find stream information." << std::endl;
    }
    av_dump_format(this->pFormatCtx, 0, this->mDeviceName.toStdString().c_str(), 0);

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

    /* 拿到video流中的 codec ctx 信息 */
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
    this->pFrameYUV = av_frame_alloc();  //统一转换为 yuv420p
    this->pFrameYUV->format = AV_PIX_FMT_YUV420P;
    this->pFrameYUV->width = this->pCodecCtx->width;
    this->pFrameYUV->height = this->pCodecCtx->height;
    ret = av_frame_get_buffer(this->pFrameYUV, 32);   //32字节对齐. ffmpeg/doc/examples/muxing.c 里面就是这样分配的
    if (ret < 0) {
        fprintf(stderr, "error when av_frame_get_buffer for YUV\n");
    }

    this->img_convert_ctx = sws_getContext( this->pCodecCtx->width, this->pCodecCtx->height, this->pCodecCtx->pix_fmt,
                                            this->pCodecCtx->width, this->pCodecCtx->height, (AVPixelFormat)this->pFrameYUV->format,
                                            SWS_BICUBIC, NULL, NULL, NULL); 
}

ffCameraReader::~ffCameraReader() {
    std::cout << "destruct ffCameraReader" << std::endl;
    if (this->img_convert_ctx) {
        sws_freeContext(this->img_convert_ctx);
    }
    if (this->pFrameYUV) {
        av_frame_free(&this->pFrameYUV);
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

QImage ffCameraReader::readFrame() {
    int ret = 0;
    char filename_buf[1024];
    if (av_read_frame(this->pFormatCtx, this->pkt) >= 0) {
        if (this->pkt->stream_index == this->videoIndex) {

            //最新的decode方法
            ret = avcodec_send_packet(this->pCodecCtx, this->pkt);
            if (ret < 0) {
                fprintf(stderr, "Error sending a packet for decoding\n");
                return QImage();
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(this->pCodecCtx, this->pFrame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    break;
                else if (ret < 0) {
                    fprintf(stderr, "Error during decoding\n");
                    break;
                }
                sws_scale(  this->img_convert_ctx, (uint8_t const* const*)this->pFrame->data, this->pFrame->linesize,
                            0, pCodecCtx->height,
                            this->pFrameYUV->data, this->pFrameYUV->linesize);

                snprintf(filename_buf, sizeof(filename_buf), "%s-%d", "decoded", this->pCodecCtx->frame_number);
                pgm_save(pFrameYUV->data[0], pFrameYUV->linesize[0], pFrameYUV->width, pFrameYUV->height, filename_buf);
            }
        }
    }
    return QImage();

    //QImage *myImage = new QImage(pFrameYUV->width, pFrameYUV->height, QImage::Format_RGB32);
    //uint8_t *src = (uint8_t *)(pFrameYUV->data[0]);
    //for (int y = 0; y < pFrameYUV->height; y++) {
    //    QRgb *scanLine = (QRgb *) myImage->scanLine(y);
    //    for (int x = 0; x < pFrameYUV->width; x++) {
    //        scanLine[x] = qRgb(src[3*x], src[3*x+1], src[3*x+2]);
    //    }
    //    src += pFrameRGB->linesize[0];
    //}
}


