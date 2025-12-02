//
// Created by root on 2025/11/10.
//

#include "../include/media.h"


auto input_path = "rtsp://192.168.3.115/video1";
using namespace std;

int main()
{
    av_log_set_level(AV_LOG_DEBUG); // 日志级别：AV_LOG_ERROR < AV_LOG_INFO < AV_LOG_DEBUG
    av_log_set_callback([](void* ptr, int level, const char* fmt, va_list vl)
    {
        if (level <= av_log_get_level())
        {
            vprintf(fmt, vl);
            printf("\n");
        }
    });


    AVFormatContext* fmt_ctx = nullptr; // 格式上下文
    const AVCodec* codec = nullptr; // 解码器
    AVCodecContext* codec_ctx = nullptr; // 解码器上下文
    AVCodecParserContext* codec_parser_context = nullptr;
    AVFrame* frame = nullptr; // 解码后帧
    AVPacket* pkt = nullptr; // 编码数据包
    SwsContext* sws_ctx = nullptr; // 图像格式转换上下文
    SwrContext* swr_ctx = nullptr;


    // mux/demux
    int ret = 0;
    ret = avformat_open_input(&fmt_ctx, input_path, nullptr, nullptr);
    ret = avformat_find_stream_info(fmt_ctx, nullptr); // 查找流信息

    int video_stream_idx = -1; // 视频流索引
    for (int i = 0; i < fmt_ctx->nb_streams; i++)
    {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_stream_idx = i;
            break;
        }
    }
    if (video_stream_idx == -1)
    {
        cerr << "No video stream found in input file" << endl;
        return -1;
    }


    // encode/decode
    AVCodecParameters* codec_par = nullptr;
    codec = avcodec_find_decoder(codec_par->codec_id); // 寻找解码器
    // codec = avcodec_find_encoder(codec_ctx->codec_id); // 寻找编码器
    if (!codec)
    {
        cerr << "Codec not found (codec_id: " << codec_par->codec_id << ")" << endl;
        return -1;
    }

    codec_ctx = avcodec_alloc_context3(codec); // 初始化编解码器
    if (!codec_ctx)
    {
        cerr << "avcodec_alloc_context3 failed" << endl;
        return -1;
    }

    ret = avcodec_parameters_to_context(codec_ctx, codec_par); // 将流参数复制到解码器上下文
    ret = avcodec_open2(codec_ctx, codec, nullptr); // 打开解码器


    pkt = av_packet_alloc(); // 分配数据包（存储编码数据）
    frame = av_frame_alloc(); // 分配帧（存储解码后原始数据）

    if (!pkt || !frame)
    {
        cerr << "av_packet_alloc / av_frame_alloc failed" << endl;
        return -1;
    }


    // ----------------------------------------------------------------------------
    while (av_read_frame(fmt_ctx, pkt) == 0)
    {
        // 读取一个数据包
        // 只处理视频流（跳过音频流等）
        if (pkt->stream_index != video_stream_idx)
        {
            av_packet_unref(pkt); // 释放数据包引用
            continue;
        }

        // 发送数据包到解码器
        ret = avcodec_send_packet(codec_ctx, pkt);
        if (ret == AVERROR(EAGAIN))
        {
            // 解码器已满，需先读取帧
            cerr << "avcodec_send_packet: EAGAIN" << endl;
            continue;
        }

        // 接收解码后的帧（可能返回多帧，需循环读取）
        while (avcodec_receive_frame(codec_ctx, frame) == 0)
        {
            // 5.3 图像格式转换：将解码后的帧（可能是 H.264 原始格式）转为 YUV420P
            sws_ctx = sws_getContext(
                codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt, // 输入尺寸、格式
                codec_ctx->width, codec_ctx->height, AV_PIX_FMT_YUV420P, // 输出尺寸、格式（YUV420P）
                SWS_BILINEAR, // 缩放算法（双线性插值）
                nullptr, nullptr, nullptr
            );
            if (!sws_ctx)
            {
                cerr << "sws_getContext failed" << endl;
                break;
            }

            // 分配输出帧缓存（存储转换后的 YUV420P 数据）
            AVFrame* yuv_frame = av_frame_alloc();
            yuv_frame->format = AV_PIX_FMT_YUV420P;
            yuv_frame->width = codec_ctx->width;
            yuv_frame->height = codec_ctx->height;
            ret = av_frame_get_buffer(yuv_frame, 0); // 分配缓冲区

        }
    }
}
