#include "dpxdecodeworker.h"
#include <QDebug>

DpxDecodeWorker::DpxDecodeWorker(QString filePath, int index)
{
    path_ = filePath;
    index_ = index;
}

void DpxDecodeWorker::run()
{
    SwsContext* swsCtx = nullptr;
    AVPixelFormat srcFmt = AV_PIX_FMT_NONE;
    AVPixelFormat dstFmt = AV_PIX_FMT_NONE;
    AVFormatContext* fmtCtx;
    AVCodec* codec;
    AVCodecContext* codecCtx;
    AVStream* stream;
//        // 生成变量
    int index;
    AVPacket pkt;
    AVFrame* frame = av_frame_alloc();
    AVFrame* output = av_frame_alloc();

    fmtCtx = avformat_alloc_context();
    if(avformat_open_input(&fmtCtx, path_.toLocal8Bit().data(),
                           NULL, NULL) < 0) {
        return;
    }
    if (avformat_find_stream_info(fmtCtx, NULL) < 0) {
        return;
    }
    for (unsigned int i = 0; i < fmtCtx->nb_streams; i++) {
        if (fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            index = i;
            continue;
        }
    }
    if (index < 0) {
        return;
    }
    stream = fmtCtx->streams[index];
    // 配置packet舍弃策略
    fmtCtx->streams[index]->discard = AVDISCARD_DEFAULT;
    // 根据视频流创建视频解码器上下文
    codecCtx = avcodec_alloc_context3(NULL);
    if (avcodec_parameters_to_context(
                codecCtx,
                fmtCtx->streams[index]->codecpar) < 0) {
        return;
    }
    // 寻找视频解码器
    codec = avcodec_find_decoder(codecCtx->codec_id);
    if (codec == NULL) {
        return;
    }
    // 打开视频解码器待用
    if (avcodec_open2(codecCtx, codec, NULL) < 0) {
        return;
    }
    av_read_frame(fmtCtx, &pkt);
    avcodec_send_packet(codecCtx, &pkt);
    avcodec_receive_frame(codecCtx, frame);
    if (srcFmt == AV_PIX_FMT_NONE) {
        srcFmt = (AVPixelFormat)frame->format;
        dstFmt = AV_PIX_FMT_RGBA64LE;
    }
    if (!(frame->width < 0 || frame->height < 0)) {
        if (swsCtx == nullptr) {
            swsCtx = sws_alloc_context();
            swsCtx = sws_getContext(frame->width,
                                    frame->height,
                                    srcFmt,
                                    frame->width,
                                    frame->height,
                                    dstFmt,
                                    SWS_BICUBIC, nullptr,
                                    nullptr, nullptr);
        }
        cv::Mat mat(frame->width, frame->height, CV_16UC4);
        SwsContext *swsCtx = sws_getContext(frame->width, frame->height,
                                            srcFmt,
                                            frame->width, frame->height,
                                            dstFmt,
                                            SWS_BICUBIC, NULL, NULL, NULL);
        av_image_fill_arrays(output->data, output->linesize, (uint8_t *)mat.data,
                             dstFmt,
                             frame->width, frame->height, 1);
        sws_scale(swsCtx,(const uint8_t* const*)frame->data, frame->linesize,
                  0, frame->height, output->data, output->linesize);
        emit decodeFinished(index_, mat);
        qDebug() << "Decode Dpx:" + path_;
    }
    this->deleteLater();
}
