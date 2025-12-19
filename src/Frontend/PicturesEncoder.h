#ifndef PICTURES2MP4_PICTURES_ENCODER_H
#define PICTURES2MP4_PICTURES_ENCODER_H
#include <cstdint>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <multimedia/player_framework/native_avmuxer.h>
#include <multimedia/player_framework/native_avsource.h>
#include <multimedia/player_framework/native_avbuffer.h>
#include <multimedia/player_framework/native_avcodec_base.h>
struct CodecBufferInfo {
    CodecBufferInfo(uint32_t index, OH_AVBuffer *buffer) : index(index), buffer(buffer), isValid(true) {}
    uint32_t index = 0;
    bool isValid = true;
    OH_AVBuffer *buffer = nullptr;
};
struct EncoderInfo {
    int64_t pts{0};
    std::string filePath;
    int32_t outputFd{-1};
    int32_t videoTrackId{-1};
    int32_t vidWidth{-1};
    int32_t vidHeight{-1};
    int32_t vidWidthStride{-1};
    int32_t vidHeightStride{-1};
    OH_AVPixelFormat pixelFormat{OH_AVPixelFormat::AV_PIXEL_FORMAT_RGBA};
};
struct CodecUserData {
public:
    std::mutex inMutex;
    std::condition_variable inCond;
    std::queue<CodecBufferInfo> inQueue;
    std::mutex outMutex;
    std::condition_variable outCond;
    std::queue<CodecBufferInfo> outQueue;
};
class PicturesEncoder {
public:
    PicturesEncoder() = default;
    ~PicturesEncoder() = default;
    static PicturesEncoder &GetInstance()
    {
        static PicturesEncoder picEncoder;
        return picEncoder;
    }
    bool Init(std::string filePath, int32_t width, int32_t height); // 初始化
    void AddPicture(uint8_t *data, size_t byteLength);              // 添加位图数据至待编码队列
    bool Start();                                                   // 启动编码器
private:
    bool InitMuxer(std::string path, int32_t width, int32_t height);
    bool InitEncoder(int32_t width, int32_t height, int32_t pixelFormat);
    bool StartMuxer();
    bool StartEncoder();
    void VideoEncInput();
    void VideoEncOutput();
    void StartRelease();
    void Release();
    std::mutex mutex_;
    std::atomic<bool> isStart_{false};
    std::atomic<bool> isFirstFrame{true};
    std::deque<std::vector<uint8_t>> pictures; // 待编码队列
    EncoderInfo encInfo_;
    CodecUserData userData_;
    OH_AVMuxer *muxer_{nullptr};
    OH_AVCodec *videoEnc_{nullptr};
    std::unique_ptr<std::thread> inputThread_{nullptr};
    std::unique_ptr<std::thread> outputThread_{nullptr};
    std::unique_ptr<std::thread> releaseThread_{nullptr};
};
#endif