//
// Created by root on 2025/11/10.
//

#include "media.h"


int main()
{
    // 存储 “协议 + 封装格式” 信息，管理所有流
    AVFormatContext av_format_context = {};

    // 单个流（音频 / 视频 / 字幕）的详细信息（编码、时间戳）
    AVStream av_stream = {};

    // 解码器 / 编码器的参数和状态
    AVCodecContext av_codec_context = {};

    // 存储解封装后的 “裸流数据包”（未解码）
    AVPacket av_packet = {};

    // 存储解码后的 “原始数据”（视频 YUV / 音频 PCM）
    AVFrame av_frame = {};

    AVCodecID av_codec_id = AV_CODEC_ID_H265;

    // 设置日志等级
    av_log_set_level(AV_LOG_VERBOSE);


    // 网络协议初始化
    // avformat_network_init();

    // 打开输入流
    avformat_open_input(&fmt_ctx, input_url, NULL, NULL);

    // 查找流信息
    avformat_find_stream_info();

    // 初始化解码器
    avcodec_alloc_context3();
    avcodec_parameters_to_context();
    avcodec_open2();

    // 读包解码
    av_read_frame();
    avcodec_send_packet(); // 发送编码包
    avcodec_receive_frame(); // 接收解码帧

    // // 创建输出流
    // avformat_new_stream();
    //
    // // 写文件头
    // avformat_write_header();
    //
    // // 查找解码器
    // avcodec_find_decoder();
    //
    // // 视频格式转换
    // sws_scale();
    //
    // // 音频格式转换
    // swr_convert();
    // av_interleaved_write_frame(); // 写帧
}
