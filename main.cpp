#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>

extern "C" {
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
}

// 函数声明
void save_frame(AVFrame *frame, int width, int height, int frame_number, const std::string &output_folder);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_video> <output_folder>" << std::endl;
        return 1;
    }

    const std::string input_video = argv[1];
    const std::string output_folder = argv[2];

    AVFormatContext *format_ctx = nullptr;
    if (avformat_open_input(&format_ctx, input_video.c_str(), nullptr, nullptr) != 0) {
        std::cerr << "Error: Couldn't open input file." << std::endl;
        return 1;
    }

    if (avformat_find_stream_info(format_ctx, nullptr) < 0) {
        std::cerr << "Error: Couldn't find stream information." << std::endl;
        avformat_close_input(&format_ctx);
        return 1;
    }

    AVCodec *codec = nullptr;
    AVCodecParameters *codec_params = nullptr;
    AVCodecContext *codec_ctx = nullptr;
    int video_stream_index = av_find_best_stream(format_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, const_cast<const AVCodec **>(&codec), 0);

    if (video_stream_index < 0) {
        std::cerr << "Error: Couldn't find a video stream." << std::endl;
        avformat_close_input(&format_ctx);
        return 1;
    }

    codec_params = format_ctx->streams[video_stream_index]->codecpar;
    codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        std::cerr << "Error: Couldn't allocate codec context." << std::endl;
        avformat_close_input(&format_ctx);
        return 1;
    }

    if (avcodec_parameters_to_context(codec_ctx, codec_params) < 0) {
        std::cerr << "Error: Couldn't initialize codec context." << std::endl;
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return 1;
    }

    if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
        std::cerr << "Error: Couldn't open codec." << std::endl;
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return 1;
    }

    AVFrame *frame = av_frame_alloc();
    AVPacket packet;
    int frame_number = 0;

    // 截取的帧数
    const int num_frames_to_save = 9;

    // 逐帧读取视频
    while (av_read_frame(format_ctx, &packet) >= 0) {
        if (packet.stream_index == video_stream_index) {
            avcodec_send_packet(codec_ctx, &packet);
            int ret = avcodec_receive_frame(codec_ctx, frame);
            if (ret == 0) {
                if (frame_number < num_frames_to_save) {
                    // 保存截取的帧
                    save_frame(frame, codec_ctx->width, codec_ctx->height, frame_number, output_folder);
                    frame_number++;
                } else {
                    break; // 截取足够数量的帧后退出循环
                }
            }
        }
        av_packet_unref(&packet);
    }

    // 释放资源
    av_frame_free(&frame);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&format_ctx);

    return 0;
}

void save_frame(AVFrame *frame, int width, int height, int frame_number, const std::string &output_folder) {
    std::ostringstream filename;
    filename << output_folder << "/frame_" << frame_number << ".png";

    FILE *file = fopen(filename.str().c_str(), "wb");
    if (!file) {
        std::cerr << "Error: Failed to open file for writing." << std::endl;
        return;
    }

    // 分配图像缓冲区
    av_frame_get_buffer(frame, 32); // Allocate frame buffer

    // 使用SWSContext执行图像转换
    struct SwsContext *sws_ctx = sws_getCachedContext(nullptr, width, height, (AVPixelFormat)frame->format,
                                                      width, height, AV_PIX_FMT_RGB24, 0, nullptr, nullptr, nullptr);
    if (!sws_ctx) {
        std::cerr << "Error: Couldn't initialize SWSContext." << std::endl;
        fclose(file);
        return;
    }

    // 分配临时缓冲区
    uint8_t *rgb_buffer = (uint8_t *)av_malloc(width * height * 3 * sizeof(uint8_t));

    // 将帧数据转换为RGB格式
    uint8_t *dst_data[1] = {rgb_buffer};
    int dst_linesize[1] = {width * 3};
    sws_scale(sws_ctx, frame->data, frame->linesize, 0, height, dst_data, dst_linesize);

    // 将RGB数据写入文件
    fwrite(rgb_buffer, 1, width * height * 3, file);

    // 释放内存
    av_freep(&rgb_buffer);
    fclose(file);

    std::cout << "Saved frame " << frame_number << std::endl;
}
