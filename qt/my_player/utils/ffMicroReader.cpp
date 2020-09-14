
#include "ffMicroReader.h"
#include <iostream>


ffMicroReader::ffMicroReader(QString path) {
    std::cout << "construct ffMicroReader, file=" << path.toStdString() << std::endl;
    int i = 0;
    AVCodec *pCodec = NULL;

    this->mDeviceName = "default";

    avdevice_register_all();    //Register Device

    this->pkt = av_packet_alloc();;
    this->pFormatCtx = avformat_alloc_context();

    /* 打开输入流 并查找基本信息 */
    AVInputFormat *ifmt = av_find_input_format("alsa");
    if (avformat_open_input(&(this->pFormatCtx), this->mDeviceName.toStdString().c_str(), ifmt, NULL) != 0) {
        std::cout << "Couldn't open input stream." << std::endl;
        goto exit;
    }

    if (avformat_find_stream_info(this->pFormatCtx, NULL) < 0) {
        std::cout << "Couldn't find stream information." << std::endl;
        goto exit;
    }
    std::cout << "format info :" << std::endl;
    av_dump_format(this->pFormatCtx, 0, this->mDeviceName.toStdString().c_str(), 0);

    /* 查找 audio */
    this->audioindex = -1;
    for (i = 0; i < this->pFormatCtx->nb_streams; i++) {
        if (this->pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            this->audioindex = i;
            break;
        }
    }
    if (this->audioindex == -1) {
        std::cout << "Couldn't find a audio stream." << std::endl;
        goto exit;
    }

    /* 获取 audio codec ctx */
    pCodec = avcodec_find_decoder(this->pFormatCtx->streams[this->audioindex]->codecpar->codec_id);
    if (!pCodec) {
        std::cout << "Codec not found." << std::endl;
        goto exit;
    }
    this->pCodecCtx = avcodec_alloc_context3(pCodec); //为 AVCodec 分配一个全新的 AVCodecContext
    if (!this->pCodecCtx) {
        std::cout << "avcodec_alloc_context3 fail." << std::endl;
        goto exit;
    }
    //从流 中拷贝信息到 codec ctx
    if (avcodec_parameters_to_context(this->pCodecCtx, this->pFormatCtx->streams[this->audioindex]->codecpar) < 0) {
        std::cout << "avcodec_parameters_to_context fail." << std::endl;
        goto exit;
    }

    if (avcodec_open2(this->pCodecCtx, pCodec, NULL) < 0) {
        std::cout << "Could not open codec." << std::endl;
        goto exit;
    }

    printf("audio codec info:\n");
    printf("sample_fmt %d\n", this->pCodecCtx->sample_fmt);   //1(AV_SAMPLE_FMT_S16)
    printf("frame_size %d\n", this->pCodecCtx->frame_size);   //4
    printf("sample_rate %d\n", this->pCodecCtx->sample_rate); //48000
    printf("channel_layout %ld\n", av_get_default_channel_layout(this->pCodecCtx->channels)); //3(AV_CH_LAYOUT_STEREO)

    this->pFrame = av_frame_alloc();     //原始 frame, 鬼知道什么格式


#if OUTPUT_PCM
    this->pFile = fopen("dump_s16le_ch2_48000hz.pcm", "wb");  //播放的时候就 ffplay test.pcm -f s16le -channels 2 -ar 48000
#endif

exit:
    std::cout << "construct exit." << std::endl;
}

ffMicroReader::~ffMicroReader() {
#if OUTPUT_PCM
    fclose(this->pFile);
#endif
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

void ffMicroReader::readFrames() {
    if (av_read_frame(this->pFormatCtx, this->pkt) >= 0) {
        if (this->pkt->stream_index == this->audioindex) {
            //最新的decode方法
            int ret = avcodec_send_packet(this->pCodecCtx, this->pkt);
            if (ret < 0) {
                std::cout << "Error sending a packet for decoding." << std::endl;
                return;
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(this->pCodecCtx, this->pFrame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                } else if (ret < 0) {
                    std::cout << "Error during decoding." << std::endl;
                    break;
                }
                //int channels = av_get_channel_layout_nb_channels(pFrameOUT->channel_layout);
                //int buffer_size = av_samples_get_buffer_size(NULL, channels, pFrame->nb_samples, pCodecCtx->sample_fmt ,1);
                //printf("get no.%3d decoded frame, nb_samples %d, channels %d bufsize %d, linesize[0] %d\n", pCodecCtx->frame_number, pFrame->nb_samples, channels, buffer_size, pFrame->linesize[0]);
#if OUTPUT_PCM
                fwrite(this->pFrame->data[0], sizeof(char), this->pFrame->linesize[0], this->pFile);
#endif
                AFrame audio_frame;
                audio_frame.len = this->pFrame->linesize[0];
                if (audio_frame.len > 0) {
                    memcpy(audio_frame.data, this->pFrame->data[0], audio_frame.len *sizeof(uint8_t));
                    emit sigGotFrame(audio_frame);
                }
                //the picture is allocated by the decoder. no need to free it
            }
        }
    }
    //usleep(400);   //400us
//    //flush decoder
//    this->pkt->data = NULL;
//    this->pkt->size = 0;
//    ret = avcodec_send_packet(this->pCodecCtx, this->pkt);
//    if (ret < 0) {
//        fprintf(stderr, "Error sending a packet for decoding\n");
//        return -1;
//    }
//    while (ret >= 0) {
//        ret = avcodec_receive_frame(pCodecCtx, pFrame);
//        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
//            break;
//        else if (ret < 0) {
//            fprintf(stderr, "Error during decoding\n");
//            break;
//        }
//
//        printf("saving frame %3d\n", pCodecCtx->frame_number);
//
//#if OUTPUT_PCM
//        fwrite(pFrame->data[0], sizeof(char), pFrame->linesize[0], pFile);
//#endif
//    }
}


