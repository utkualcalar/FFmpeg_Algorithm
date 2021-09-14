//Yasar Utku Alcalar

#include "FFmpeg.h"
#include "QTextDecoder"

FFmpeg::FFmpeg()
{
    pCodecCtx = NULL;
    videoStream=-1;
    ready=false;
}

FFmpeg::~FFmpeg()
{
    sws_freeContext(pSwsCtx);
}
#include <QDebug>
int FFmpeg::initial(QString & url)
{
    int err;
    rtspURL=url;
    AVCodec *pCodec;
    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();
    pFrame=av_frame_alloc();

    AVDictionary *opts = NULL;

    //av_dict_set(&opts, "allowed_media_types", "video", 0);
    av_dict_set_int(&opts, "rw_timeout", 5000000, 0);
    //av_dict_set_int(&opts, "max_delay", 50, 0);
    av_dict_set_int(&opts, "tcp_nodelay", 1, 0);
    av_dict_set_int(&opts, "stimeout", 10000000, 0);
    av_dict_set(&opts, "rtsp_transport", "tcp", 0);
    av_dict_set(&opts, "rtsp_flags", "prefer_tcp", 0);
    av_dict_set_int(&opts, "buffer_size", 10000, 0);


    err = avformat_open_input(&pFormatCtx, rtspURL.toUtf8().data(), NULL, &opts);


    if (err < 0)
    {
        qDebug("Can not open this file");
        return -1;
    }

    if (avformat_find_stream_info(pFormatCtx,NULL) < 0)
    {
        qDebug("Unable to get stream info");
        return -1;
    }

    int i = 0;
    videoStream = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }
    if (videoStream == -1) {
        qDebug("Unable to find video stream");
        return -1;
    }
    pCodecCtx = pFormatCtx->streams[videoStream]->codec;

    width=pCodecCtx->width;
    height=pCodecCtx->height;
    avpicture_alloc(&picture, AV_PIX_FMT_RGB24,pCodecCtx->width,pCodecCtx->height);
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    pSwsCtx = sws_getContext(width, height, AV_PIX_FMT_YUV420P, width, height, AV_PIX_FMT_RGB24, SWS_FAST_BILINEAR, 0, 0, 0);

    if (pCodec == NULL) {
        qDebug("Unsupported codec");
        return -1;
    }
    qDebug("video size : width=%d height=%d \n", pCodecCtx->width,
           pCodecCtx->height);
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) 	{
        qDebug("Unable to open codec");
        return -1;
    }
    qDebug("initial successfully");
    return 0;
}

#include <unistd.h>
int FFmpeg::h264Decodec()
{
    int frameFinished=0;
    while (av_read_frame(pFormatCtx, &packet) >= 0)
    {
        if(packet.stream_index==videoStream)
        {
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
            if (frameFinished)
            {
                mutex.lock();
                int rs = sws_scale(pSwsCtx, (const uint8_t* const *) pFrame->data, pFrame->linesize, 0, height, picture.data, picture.linesize);
                if (rs == -1)
                {
                    qDebug("__________Can open to change to des imag_____________e\n");
                    return -1;
                }
                ready = true;
                mutex.unlock();
                usleep(20000);
            }
        }
    }
    return 0;
}
