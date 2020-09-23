
#include "fileDmx.h"
#include <iostream>


fileDmx::fileDmx(QString path) {
    std::cout << "construct fileDmx, file=" << path.toStdString() << std::endl;
    int ret = 0;
    this->mFileName = path;

    //avformat_network_init();    //? 似乎不需要
    //avdevice_register_all();    //Register Device

    this->pFormatCtx = avformat_alloc_context();
    this->pkt = av_packet_alloc();

    /* 1.打开format并获取其中的stream信息, param3 给的 NULL: av_guess_format */
    if (avformat_open_input(&(this->pFormatCtx), this->mFileName.toStdString().c_str(), NULL, NULL) != 0) {
        std::cout << "Couldn't open input stream." << std::endl;
    }

    if (avformat_find_stream_info(this->pFormatCtx, NULL) < 0) {
        std::cout << "Couldn't find stream information." << std::endl;
    }

    /* 2.完成后做一个打印 */
    av_dump_format(this->pFormatCtx, 0, this->mFileName.toStdString().c_str(), 0);

    /* 3.查找video流 */
    this->videoIndex = -1;
    this->audioIndex = -1;
    for (int i = 0; i < this->pFormatCtx->nb_streams; i++) {
        if (this->pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            this->videoIndex = i;
        }
        if (this->pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            this->audioIndex = i;
        }
    }
    if (this->videoIndex == -1 || this->audioIndex == -1) {
        std::cout << "Couldn't find a video/audio stream." << std::endl;
    }

    /* 4.拿到video流中的 codec ctx 信息 */
    AVCodec *pVCodec = avcodec_find_decoder(this->pFormatCtx->streams[this->videoIndex]->codecpar->codec_id);
    AVCodec *pACodec = avcodec_find_decoder(this->pFormatCtx->streams[this->audioIndex]->codecpar->codec_id);
    if (!pVCodec || !pACodec) {
        std::cout << "Codec not found." << std::endl;
    }
    this->pVCodecCtx = avcodec_alloc_context3(pVCodec); //为 AVCodec 分配一个全新的 AVCodecContext
    this->pACodecCtx = avcodec_alloc_context3(pACodec); //为 AVCodec 分配一个全新的 AVCodecContext
    if (!this->pVCodecCtx || !this->pACodecCtx) {
        std::cout << "avcodec_alloc_context3 fail." << std::endl;
    }
    //拷贝 原 stream 中的 codecpar 信息
    if (avcodec_parameters_to_context(this->pVCodecCtx, this->pFormatCtx->streams[this->videoIndex]->codecpar) < 0 ||
        avcodec_parameters_to_context(this->pACodecCtx, this->pFormatCtx->streams[this->audioIndex]->codecpar) < 0) {
        std::cout << "avcodec_parameters_to_context fail." << std::endl;
    }

    if (avcodec_open2(this->pVCodecCtx, pVCodec, NULL) < 0 ||
        avcodec_open2(this->pACodecCtx, pACodec, NULL) < 0) {
        std::cout << "Could not open codec." << std::endl;
    }
    printf("video codec info:\n");
    printf("sample_fmt %d\n", this->pVCodecCtx->width);
    printf("frame_size %d\n", this->pVCodecCtx->height);
    printf("pix_fmt:%s\n", av_get_pix_fmt_name(this->pVCodecCtx->pix_fmt));

    printf("audio codec info:\n");
    printf("sample_fmt %s\n", av_get_sample_fmt_name(this->pACodecCtx->sample_fmt));
    //printf("frame_size %d\n", this->pACodecCtx->frame_size);
    printf("sample_rate %d\n", this->pACodecCtx->sample_rate);
    //printf("channel_layout %ld\n", av_get_default_channel_layout(this->pACodecCtx->channels));
    printf("channel %d\n", this->pACodecCtx->channels); //3(AV_CH_LAYOUT_STEREO)

    this->pFrame = av_frame_alloc();     //原始 frame, 鬼知道什么格式
    this->pFrame4QImage = av_frame_alloc();  //统一转换为 yuv420p
    this->pFrame4QImage->format = AV_PIX_FMT_YUV420P;//AV_PIX_FMT_RGB24; QImage 只支持rgb格式
    this->pFrame4QImage->width = this->pVCodecCtx->width;
    this->pFrame4QImage->height = this->pVCodecCtx->height;
    ret = av_frame_get_buffer(this->pFrame4QImage, 32);   //32字节对齐. ffmpeg/doc/examples/muxing.c 里面就是这样分配的
    if (ret < 0) {
        fprintf(stderr, "error when av_frame_get_buffer for pFrame\n");
    }

    this->img_convert_ctx = sws_getContext( this->pVCodecCtx->width, this->pVCodecCtx->height, this->pVCodecCtx->pix_fmt,
                                            this->pVCodecCtx->width, this->pVCodecCtx->height, (AVPixelFormat)this->pFrame4QImage->format,
                                            SWS_BICUBIC, NULL, NULL, NULL); 
    this->pFileYuv = fopen("test_out.yuv", "wb");
    this->pFilePcm = fopen("test_out.pcm", "wb");
}

fileDmx::~fileDmx() {
    std::cout << "destruct fileDmx" << std::endl;
    fclose(this->pFilePcm);
    fclose(this->pFileYuv);
    if (this->img_convert_ctx) {
        sws_freeContext(this->img_convert_ctx);
    }
    if (this->pFrame4QImage) {
        av_frame_free(&this->pFrame4QImage);
    }
    if (this->pFrame) {
        av_frame_free(&this->pFrame);
    }
    if (this->pVCodecCtx) {
        avcodec_close(this->pVCodecCtx);
        avcodec_free_context(&pVCodecCtx);
    }
    if (this->pACodecCtx) {
        avcodec_close(this->pACodecCtx);
        avcodec_free_context(&pACodecCtx);
    }
    if (this->pFormatCtx) {
        avformat_close_input(&pFormatCtx);//including avformat_free_context
    }
    if (this->pkt) {
        av_packet_free(&pkt);
    }
}

int fileDmx::readFrames() {
    int ret = 0;
    if (av_read_frame(this->pFormatCtx, this->pkt) >= 0) {
        if (this->pkt->stream_index == this->videoIndex) {

            //最新的decode方法
            ret = avcodec_send_packet(this->pVCodecCtx, this->pkt);
            if (ret < 0) {
                std::cout << "Error sending a video packet for decoding." << std::endl;
                return -1;
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(this->pVCodecCtx, this->pFrame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                } else if (ret < 0) {
                    std::cout << "Error during decoding." << std::endl;
                    break;
                }
                sws_scale(  this->img_convert_ctx, (uint8_t const* const*)this->pFrame->data, this->pFrame->linesize,
                            0, pVCodecCtx->height,
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
                emit sigGotVideoFrame(img.copy());

                for (int i = 0; i < pFrame4QImage->height; i++) {
                    fwrite(pFrame4QImage->data[0] + i * pFrame4QImage->linesize[0], 1, pFrame4QImage->width, this->pFileYuv);
                }
                for (int i = 0; i < pFrame4QImage->height/2; i++) {
                    fwrite(pFrame4QImage->data[1] + i * pFrame4QImage->linesize[1], 1, pFrame4QImage->width/2, this->pFileYuv);
                }
                for (int i = 0; i < pFrame4QImage->height/2; i++) {
                    fwrite(pFrame4QImage->data[2] + i * pFrame4QImage->linesize[2], 1, pFrame4QImage->width/2, this->pFileYuv);
                }
            }
        } else if (this->pkt->stream_index == this->audioIndex) {
            //this->aFrames.append(af);   //append, prepend 都可用
        
            ret = avcodec_send_packet(this->pACodecCtx, this->pkt);
            if (ret < 0) {
                std::cout << "Error sending a audio packet for decoding." << std::endl;
                return -1;
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(this->pACodecCtx, this->pFrame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                } else if (ret < 0) {
                    std::cout << "Error during decoding." << std::endl;
                    break;
                }
                fwrite(pFrame->data[0], 1, pFrame->linesize[0], this->pFilePcm);
                AFrame audio_frame;
                audio_frame.len = this->pFrame->linesize[0];
                if (audio_frame.len > 0) {
                    memcpy(audio_frame.data, this->pFrame->data[0], audio_frame.len * sizeof(uint8_t));
                    emit sigGotAudioFrame(audio_frame);
                }
            }
        }
    }
    return 0;
}


