//
// Created by root on 2025/11/10.
//

#include "../include/media.h"
#include "libhikvision/HCNetSDK.h"

auto input_path = "rtsp://192.168.3.115/video1";
using namespace std;

int foo()
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
    return 0;
}


int controlGateway()
{
    NET_DVR_Init();
    LONG lUserID;
    NET_DVR_USER_LOGIN_INFO struLoginInfo = {0};
    struLoginInfo.bUseAsynLogin = 0; //同步登录方式
    strcpy(struLoginInfo.sDeviceAddress, "192.168.3.45"); //设备IP地址
    struLoginInfo.wPort = 8000; //设备服务端口
    strcpy(struLoginInfo.sUserName, "admin"); //设备登录用户名
    strcpy(struLoginInfo.sPassword, "xirui@528437"); //设备登录密码

    //设备信息, 输出参数
    NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = {0};

    lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);
    if (lUserID < 0)
    {
        printf("Login failed, error code: %d\n", NET_DVR_GetLastError());
        NET_DVR_Cleanup();
        return 1;
    }

    //远程控门
    int lGatewayIndex = -1; //门禁序号（楼层编号、锁ID），从1开始，-1表示对所有门（或者梯控的所有楼层）进行操作
    int dwStaic = 1; //命令值：0 - 关闭（对于梯控，表示受控），1 - 打开（对于梯控，表示开门），
    //2 - 常开（对于梯控，表示自由、通道状态），3 - 常关（对于梯控，表示禁用），
    //4 - 恢复（梯控，普通状态），5 - 访客呼梯（梯控），6 - 住户呼梯（梯控）

    if (!NET_DVR_ControlGateway(lUserID, lGatewayIndex, dwStaic))
    {
        printf("NET_DVR_ControlGateway failed, error code: %d\n", NET_DVR_GetLastError());
    }
    else
    {
        printf("NET_DVR_ControlGateway successfully!\n");
    }

    //---------------------------------------
    //退出程序

    //注销用户
    NET_DVR_Logout(lUserID);

    //释放SDK资源
    NET_DVR_Cleanup();
    return 0;
}


int iNum = 0; //已保存的图片个数

/**
 *
 * @param lCommand 上传的消息类型，不同的报警对应不同的消息类型
 * @param pAlarmer 报警设备的信息（序列号、IP、登录的userid）
 * @param pAlarmInfo 报警信息，根据lCommand的不同，其组成结构也不同
 * @param dwBufLen 报警信息缓存大小
 * @param pUser 用户数据
 */
void CALLBACK MessageCallback(LONG lCommand, NET_DVR_ALARMER* pAlarmer, char* pAlarmInfo, DWORD dwBufLen, void* pUser)
{
    switch (lCommand)
    {
    case COMM_ALARM_ACS: //门禁主机报警信息
        {
            NET_DVR_ACS_ALARM_INFO struAcsAlarmInfo = {0};
            memcpy(&struAcsAlarmInfo, pAlarmInfo, sizeof(NET_DVR_ACS_ALARM_INFO));
            printf(
                "门禁主机报警信息[0x5002]: struTim{%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d}, dwMajor[0x%x], dwMinor[0x%x], byCardNo[%s], dwEmployeeNo[%d], dwCardReaderNo[%d]\n",
                struAcsAlarmInfo.struTime.dwYear,
                struAcsAlarmInfo.struTime.dwMonth,
                struAcsAlarmInfo.struTime.dwDay,
                struAcsAlarmInfo.struTime.dwHour,
                struAcsAlarmInfo.struTime.dwMinute,
                struAcsAlarmInfo.struTime.dwSecond,
                struAcsAlarmInfo.dwMajor,
                struAcsAlarmInfo.dwMinor,
                struAcsAlarmInfo.struAcsEventInfo.byCardNo,
                struAcsAlarmInfo.struAcsEventInfo.dwEmployeeNo,
                struAcsAlarmInfo.struAcsEventInfo.dwCardReaderNo);

            //扩展信息，包含以人为中心下发人员触发事件中的工号参数
            if (struAcsAlarmInfo.byAcsEventInfoExtend == 1)
            {
                NET_DVR_ACS_EVENT_INFO_EXTEND struAcsEventInfoExtend = {0};
                memset(&struAcsEventInfoExtend, 0, sizeof(struAcsEventInfoExtend));
                memcpy(&struAcsEventInfoExtend, struAcsAlarmInfo.pAcsEventInfoExtend, sizeof(struAcsEventInfoExtend));

                printf("门禁扩展事件信息: dwFrontSerialNo[%d], byUserType[%d], byEmployeeNo[%s]\n",
                       struAcsEventInfoExtend.dwFrontSerialNo,
                       struAcsEventInfoExtend.byUserType, (char*)struAcsEventInfoExtend.byEmployeeNo);
            }

            //扩展信息，包含人体测温温度数据
            if (struAcsAlarmInfo.byAcsEventInfoExtendV20 == 1)
            {
                NET_DVR_ACS_EVENT_INFO_EXTEND_V20 struAcsEventInfoExtendV20 = {0};
                memcpy(&struAcsEventInfoExtendV20, struAcsAlarmInfo.pAcsEventInfoExtendV20,
                       sizeof(struAcsEventInfoExtendV20));

                if (struAcsEventInfoExtendV20.byRemoteCheck != 0)
                {
                    //需要远程核验
                    printf("remote check:%d\n", struAcsEventInfoExtendV20.byRemoteCheck);
                }
                if (struAcsEventInfoExtendV20.fCurrTemperature != 0)
                {
                    printf("temperature[%f]fX[%f]fY[%f]byThermometryUnit[%d]byIsAbnomalTemperature[%d]\n",
                           struAcsEventInfoExtendV20.fCurrTemperature,
                           struAcsEventInfoExtendV20.struRegionCoordinates.fX,
                           struAcsEventInfoExtendV20.struRegionCoordinates.fY,
                           struAcsEventInfoExtendV20.byThermometryUnit,
                           struAcsEventInfoExtendV20.byIsAbnomalTemperature);
                }
            }

            //保存报警抓拍图片
            if (struAcsAlarmInfo.dwPicDataLen > 0 && struAcsAlarmInfo.pPicData != NULL)
            {
                char cFilename[256] = {0};

                char chTime[128];
                sprintf(chTime, "%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d", struAcsAlarmInfo.struTime.dwYear,
                        struAcsAlarmInfo.struTime.dwMonth,
                        struAcsAlarmInfo.struTime.dwDay, struAcsAlarmInfo.struTime.dwHour,
                        struAcsAlarmInfo.struTime.dwMinute,
                        struAcsAlarmInfo.struTime.dwSecond);

                sprintf(cFilename, "COMM_ALARM_ACS_CapPic[%d][%s].jpg", pAlarmer->lUserID, chTime);

                FILE* fSnapPicPlate = fopen(cFilename, "wb");
                fwrite(struAcsAlarmInfo.pPicData, struAcsAlarmInfo.dwPicDataLen, 1, fSnapPicPlate);
                fclose(fSnapPicPlate);
                iNum++;
            }
        }
        break;
    case COMM_ID_INFO_ALARM: // 门禁身份证刷卡信息
        {
            NET_DVR_ID_CARD_INFO_ALARM struIDCardInfo = {0};
            memcpy(&struIDCardInfo, pAlarmInfo, sizeof(NET_DVR_ID_CARD_INFO_ALARM));
            printf(
                "门禁身份证刷卡信息[0x5200]: struTim{%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d}, dwMajor[0x%x], dwMinor[0x%x], byName[%s], byIDNum[%s], dwCardReaderNo[%d]\n",
                struIDCardInfo.struSwipeTime.wYear, struIDCardInfo.struSwipeTime.byMonth,
                struIDCardInfo.struSwipeTime.byDay, struIDCardInfo.struSwipeTime.byHour,
                struIDCardInfo.struSwipeTime.byMinute, struIDCardInfo.struSwipeTime.bySecond, struIDCardInfo.dwMajor,
                struIDCardInfo.dwMinor,
                (char*)struIDCardInfo.struIDCardCfg.byName, (char*)struIDCardInfo.struIDCardCfg.byIDNum,
                struIDCardInfo.dwCardReaderNo);

            //扩展信息，包含人体测温温度数据
            if (struIDCardInfo.byIDCardInfoExtend == 1)
            {
                NET_DVR_ID_CARD_INFO_EXTEND struIDCardExtendV20 = {0};
                memcpy(&struIDCardExtendV20, struIDCardInfo.pIDCardInfoExtend, sizeof(struIDCardExtendV20));

                if (struIDCardExtendV20.byRemoteCheck != 0)
                {
                    //需要远程核验
                    printf("remote check:%d\n", struIDCardExtendV20.byRemoteCheck);
                }
                if (struIDCardExtendV20.fCurrTemperature != 0)
                {
                    printf("temperature[%f]fX[%f]fY[%f]byThermometryUnit[%d]byIsAbnomalTemperature[%d]\n",
                           struIDCardExtendV20.fCurrTemperature, struIDCardExtendV20.struRegionCoordinates.fX,
                           struIDCardExtendV20.struRegionCoordinates.fY, struIDCardExtendV20.byThermometryUnit,
                           struIDCardExtendV20.byIsAbnomalTemperature);
                }
            }

            //保存身份证图片数据大小
            if (struIDCardInfo.dwPicDataLen > 0 && struIDCardInfo.pPicData != NULL)
            {
                char cCardFilename[256] = {0};

                char chTime[128];
                sprintf(chTime, "%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d", struIDCardInfo.struSwipeTime.wYear,
                        struIDCardInfo.struSwipeTime.byMonth,
                        struIDCardInfo.struSwipeTime.byDay, struIDCardInfo.struSwipeTime.byHour,
                        struIDCardInfo.struSwipeTime.byMinute, struIDCardInfo.struSwipeTime.bySecond);

                sprintf(cCardFilename, "COMM_ID_INFO_ALARM_IDCardPic[%d][%s].jpg", pAlarmer->lUserID, chTime);

                FILE* fSnapPicPlate = fopen(cCardFilename, "wb");
                fwrite(struIDCardInfo.pPicData, struIDCardInfo.dwPicDataLen, 1, fSnapPicPlate);
                fclose(fSnapPicPlate);
                iNum++;
            }

            //保存抓拍图片数据大小
            if (struIDCardInfo.dwCapturePicDataLen > 0 && struIDCardInfo.pCapturePicData != NULL)
            {
                char cFilename[256] = {0};

                char chTime[128];
                sprintf(chTime, "%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d", struIDCardInfo.struSwipeTime.wYear,
                        struIDCardInfo.struSwipeTime.byMonth,
                        struIDCardInfo.struSwipeTime.byDay, struIDCardInfo.struSwipeTime.byHour,
                        struIDCardInfo.struSwipeTime.byMinute, struIDCardInfo.struSwipeTime.bySecond);

                sprintf(cFilename, "COMM_ID_INFO_ALARM_CapPic[%d][%s].jpg", pAlarmer->lUserID, chTime);

                FILE* fSnapPicPlate = fopen(cFilename, "wb");
                fwrite(struIDCardInfo.pCapturePicData, struIDCardInfo.dwCapturePicDataLen, 1, fSnapPicPlate);
                fclose(fSnapPicPlate);
                iNum++;
            }
        }
        break;
    case COMM_ALARM_VIDEO_INTERCOM: // 门禁通行人数信息
        {
        }
        break;
    default:
        printf("其他报警，报警信息类型: %d\n", lCommand);
        break;
    }

    return;
}

int getEvent()
{
    //---------------------------------------
    // 初始化
    NET_DVR_Init();
    //设置连接时间与重连时间
    NET_DVR_SetConnectTime(2000, 1);
    NET_DVR_SetReconnect(10000, true);

    //---------------------------------------
    // 注册设备
    LONG lUserID;

    //登录参数，包括设备地址、登录用户、密码等
    NET_DVR_USER_LOGIN_INFO struLoginInfo = {0};
    struLoginInfo.bUseAsynLogin = 0; //同步登录方式
    strcpy(struLoginInfo.sDeviceAddress, "192.168.3.45"); //设备IP地址
    struLoginInfo.wPort = 8000; //设备服务端口
    strcpy(struLoginInfo.sUserName, "admin"); //设备登录用户名
    strcpy(struLoginInfo.sPassword, "xirui@528437"); //设备登录密码

    //设备信息, 输出参数
    NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = {0};

    lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);
    if (lUserID < 0)
    {
        printf("Login failed, error code: %d\n", NET_DVR_GetLastError());
        NET_DVR_Cleanup();
        return -1;
    }

    //设备登录成功，获取设备字符集编码类型
    printf("Login successfully, byCharEncodeType: %d\n", struDeviceInfoV40.byCharEncodeType);


    //设置报警回调函数
    NET_DVR_SetDVRMessageCallBack_V50(0, MessageCallback, NULL);

    //启用布防
    LONG lHandle;
    NET_DVR_SETUPALARM_PARAM struAlarmParam = {0};
    struAlarmParam.dwSize = sizeof(struAlarmParam);
    //不需要设置其他报警布防参数，不支持

    lHandle = NET_DVR_SetupAlarmChan_V41(lUserID, &struAlarmParam);
    if (lHandle < 0)
    {
        printf("NET_DVR_SetupAlarmChan_V41 failed, error code:  %d\n", NET_DVR_GetLastError());
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return -1;
    }

    //事件信息在回调函数里面获取
    //控制台输入q退出程序，否则一直运行
    char c = 0;
    while ('q' != c)
    {
        printf("input 'q' to quit\n");
        printf("input: ");
        scanf("%c", &c);
    }

    //---------------------------------------
    //退出程序

    //撤销布防上传通道
    if (!NET_DVR_CloseAlarmChan_V30(lHandle))
    {
        printf("NET_DVR_CloseAlarmChan_V30 failed, error code: %d\n", NET_DVR_GetLastError());
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return -1;
    }

    //注销用户
    NET_DVR_Logout(lUserID);

    //释放SDK资源
    NET_DVR_Cleanup();
    return 0;
}

int main(int argc, char* argv[])
{
    return getEvent();
}
