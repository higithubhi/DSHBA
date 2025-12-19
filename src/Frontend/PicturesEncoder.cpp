#include "PicturesEncoder.h"
#include <cassert>
#include <cstdint>
#include <hilog/log.h>
#include <fcntl.h>
#include <unistd.h>
#include <multimedia/player_framework/native_avcodec_videoencoder.h>
#include <multimedia/player_framework/native_avcapability.h>
#include <multimedia/player_framework/native_avcodec_base.h>
#include <multimedia/player_framework/native_averrors.h>
#include <multimedia/player_framework/native_avformat.h>
#include <multimedia/player_framework/native_avbuffer.h>
#undef LOG_TAG
#undef LOG_DOMAIN
#define LOG_DOMAIN 0x3200
#define LOG_TAG "Pictures Encoder"
namespace {
constexpr int64_t MICROSECOND = 1000000; // 1S
}
/**
 * 添加位图数据到待编码队列
 * @param data 位图数据
 * @param byteLength 缓冲区长度
 */
void PicturesEncoder::AddPicture(uint8_t *data, size_t byteLength)
{
    std::vector<uint8_t> pic(byteLength);
    uint8_t *buffer = pic.data();
    for (int idx = 0; idx < byteLength; idx++) {
        buffer[idx] = data[idx];
    }
    pictures.emplace_back(pic);
}
static void OnError(OH_AVCodec *codec, int32_t errCode, void *userData)
{
    (void)codec;
    (void)userData;
    OH_LOG_ERROR(LOG_APP, "video encoder error: code is %{public}d", errCode);
}
static void OnStreamChanged(OH_AVCodec *codec, OH_AVFormat *format, void *userData)
{
    (void)codec;
    (void)format;
    (void)userData;
    OH_LOG_INFO(LOG_APP, "video encoder stream changed");
}
static void OnNeedInputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *userData)
{
    (void)codec;
    CodecUserData *codecData = reinterpret_cast<CodecUserData *>(userData);
    std::unique_lock<std::mutex> lock(codecData->inMutex);
    (void)lock;
    codecData->inQueue.push({index, buffer});
    codecData->inCond.notify_all();
}
static void OnNewOutputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *userData)
{
    (void)codec;
    CodecUserData *codecData = reinterpret_cast<CodecUserData *>(userData);
    std::unique_lock<std::mutex> lock(codecData->outMutex);
    (void)lock;
    codecData->outQueue.push({index, buffer});
    codecData->outCond.notify_all();
}
/**
 * 编码输入线程
 */
void PicturesEncoder::VideoEncInput()
{
    bool isError{false};
    while (isStart_) {
        std::unique_lock<std::mutex> lock(userData_.inMutex);
        userData_.inCond.wait_for(lock, std::chrono::milliseconds(500),
                                  [this] { return !userData_.inQueue.empty() || !isStart_; });
        if (!isStart_) {
            break;
        }
        if (userData_.inQueue.empty()) {
            continue;
        }
        if (isFirstFrame) {
            OH_AVFormat *inputFormat = OH_VideoEncoder_GetInputDescription(videoEnc_);
            if (inputFormat == nullptr) {
                OH_LOG_ERROR(LOG_APP, "OH_VideoEncoder_GetInputDescription failed");
                isError = true;
                break;
            }
            if (OH_AVFormat_GetIntValue(inputFormat, OH_MD_KEY_VIDEO_STRIDE, &encInfo_.vidWidthStride) != true) {
                OH_LOG_ERROR(LOG_APP, "Get Width Stride Failed");
                isError = true;
                break;
            }
            if (OH_AVFormat_GetIntValue(inputFormat, OH_MD_KEY_VIDEO_SLICE_HEIGHT, &encInfo_.vidHeightStride) != true) {
                OH_LOG_ERROR(LOG_APP, "Get Height Stride Failed");
                isError = true;
                break;
            }
            OH_AVFormat_Destroy(inputFormat);
            isFirstFrame.store(false);
        }
        CodecBufferInfo bufferInfo = userData_.inQueue.front();
        userData_.inQueue.pop();
        OH_AVCodecBufferAttr attr{0};
        int32_t frameSize = encInfo_.vidWidthStride * encInfo_.vidHeightStride * 3 / 2;
        if (!pictures.empty()) {
            attr.size = frameSize;
            attr.offset = 0;
            attr.pts = 0;
            uint8_t *frame = (uint8_t *)malloc(frameSize);
            if (frame == nullptr) {
                OH_LOG_ERROR(LOG_APP, "malloc frame failed");
                isError = true;
                break;
            }
            uint8_t *frameTemp = (uint8_t *)frame;
            uint8_t *dataTemp = (uint8_t *)pictures.front().data();
            for (int32_t idx = 0; idx < encInfo_.vidHeightStride; idx++) {
                for (int32_t j = 0; j < encInfo_.vidWidth; j++) {
                    frameTemp[j] = dataTemp[j];
                }
                frameTemp += encInfo_.vidWidthStride;
                dataTemp += encInfo_.vidWidth;
            }
            frameTemp += (encInfo_.vidHeightStride - encInfo_.vidHeight) * encInfo_.vidWidthStride;
            for (int32_t idx = 0; idx < (encInfo_.vidHeight / 2); idx++) {
                for (int32_t j = 0; j < encInfo_.vidWidth; j++) {
                    frameTemp[j] = dataTemp[j];
                }
                frameTemp += encInfo_.vidWidthStride;
                dataTemp += encInfo_.vidWidth;
            }
            pictures.pop_front();
            int32_t capacity = OH_AVBuffer_GetCapacity(bufferInfo.buffer);
            if (frameSize > capacity) {
                OH_LOG_ERROR(LOG_APP, "frame size(%{public}d)> buffer capacity(%{public}d)", frameSize, capacity);
                isError = true;
                break;
            }
            uint8_t *addr = OH_AVBuffer_GetAddr(bufferInfo.buffer);
            if (addr == nullptr) {
                OH_LOG_ERROR(LOG_APP, "OH_AVBuffer_GetAddr failed");
                isError = true;
                break;
            }
            for (int32_t j = 0; j < frameSize; j++) {
                addr[j] = frame[j];
            }
        } else {
            attr.size = 0;
            attr.offset = 0;
            attr.pts = 0;
            attr.flags = AVCODEC_BUFFER_FLAGS_EOS;
        }
        int32_t ret = OH_AVBuffer_SetBufferAttr(bufferInfo.buffer, &attr);
        if (ret != AV_ERR_OK) {
            OH_LOG_ERROR(LOG_APP, "OH_AVBuffer_SetBufferAttr failed");
            isError = true;
            break;
        }
        ret = OH_VideoEncoder_PushInputBuffer(videoEnc_, bufferInfo.index);
        if (ret != AV_ERR_OK) {
            OH_LOG_ERROR(LOG_APP, "OH_VideoEncoder_PushInputBuffer failed");
            isError = true;
            break;
        }
        if (attr.flags == AVCODEC_BUFFER_FLAGS_EOS) {
            OH_LOG_INFO(LOG_APP, "VideoEncInput thread eos");
            break;
        }
    }
    OH_LOG_INFO(LOG_APP, "VideoEncInput thread exit");
    if (isError) {
        StartRelease();
    }
}
/**
 * 编码输出线程
 */
void PicturesEncoder::VideoEncOutput()
{
    while (isStart_) {
        std::unique_lock<std::mutex> lock(userData_.outMutex);
        userData_.outCond.wait_for(lock, std::chrono::milliseconds(500),
                                   [&] { return !userData_.outQueue.empty() || !isStart_; });
        if (!isStart_) {
            break;
        }
        if (userData_.outQueue.empty()) {
            continue;
        }
        CodecBufferInfo bufferInfo = userData_.outQueue.front();
        userData_.outQueue.pop();
        OH_AVCodecBufferAttr attr;
        int32_t ret = OH_AVBuffer_GetBufferAttr(bufferInfo.buffer, &attr);
        if (ret != AV_ERR_OK) {
            OH_LOG_ERROR(LOG_APP, "OH_AVBuffer_GetBufferAttr failed");
            break;
        }
        if ((attr.flags & AVCODEC_BUFFER_FLAGS_SYNC_FRAME) || attr.flags == AVCODEC_BUFFER_FLAGS_NONE) {
            attr.pts = encInfo_.pts;
            encInfo_.pts += MICROSECOND;
        } else if (attr.flags & AVCODEC_BUFFER_FLAGS_EOS) {
            attr.size = 0;
            attr.pts = encInfo_.pts;
        } else {
            attr.pts = 0;
        }
        ret = OH_AVBuffer_SetBufferAttr(bufferInfo.buffer, &attr);
        if (ret != AV_ERR_OK) {
            OH_LOG_ERROR(LOG_APP, "OH_AVBuffer_SetBufferAttr failed");
            break;
        }
        ret = OH_AVMuxer_WriteSampleBuffer(muxer_, encInfo_.videoTrackId, bufferInfo.buffer);
        if (ret != AV_ERR_OK) {
            OH_LOG_ERROR(LOG_APP, "OH_AVMuxer_WriteSampleBuffer failed");
            break;
        }
        ret = OH_VideoEncoder_FreeOutputBuffer(videoEnc_, bufferInfo.index);
        if (ret != AV_ERR_OK) {
            OH_LOG_ERROR(LOG_APP, "OH_VideoEncoder_FreeOutputBuffer failed");
            break;
        }
        if (attr.flags == AVCODEC_BUFFER_FLAGS_EOS) {
            OH_LOG_INFO(LOG_APP, "VideoEncOutput thread eos");
            break;
        }
    }
    OH_LOG_INFO(LOG_APP, "VideoEncOutput exit");
    StartRelease();
}
void PicturesEncoder::StartRelease()
{
    if (releaseThread_ == nullptr) {
        OH_LOG_INFO(LOG_APP, "start release");
        releaseThread_ = std::make_unique<std::thread>(&PicturesEncoder::Release, this);
    }
}
void PicturesEncoder::Release()
{
    std::lock_guard<std::mutex> lock(mutex_);
    (void)lock;
    isStart_.store(false);
    if (inputThread_ && inputThread_->joinable()) {
        userData_.inCond.notify_all();
        inputThread_->join();
        inputThread_.reset();
        inputThread_ = nullptr;
    }
    if (outputThread_ && outputThread_->joinable()) {
        userData_.outCond.notify_all();
        outputThread_->join();
        outputThread_.reset();
        outputThread_ = nullptr;
    }
    if (muxer_ != nullptr) {
        OH_AVMuxer_Destroy(muxer_);
        muxer_ = nullptr;
    }
    if (videoEnc_ != nullptr) {
        OH_VideoEncoder_Destroy(videoEnc_);
        videoEnc_ = nullptr;
    }
    if (encInfo_.outputFd >= 0) {
        close(encInfo_.outputFd);
        encInfo_.outputFd = -1;
    }
    while (!userData_.inQueue.empty()) {
        userData_.inQueue.pop();
    }
    while (!userData_.outQueue.empty()) {
        userData_.outQueue.pop();
    }
    pictures.clear();
    OH_LOG_INFO(LOG_APP, "release done");
}
bool PicturesEncoder::InitMuxer(std::string filePath, int32_t width, int32_t height)
{
    // create muxer
    int32_t fd = open(filePath.c_str(), O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    encInfo_.outputFd = fd;
    OH_AVOutputFormat format{AV_OUTPUT_FORMAT_MPEG_4};
    muxer_ = OH_AVMuxer_Create(fd, format);
    if (muxer_ == nullptr) {
        OH_LOG_ERROR(LOG_APP, "OH_AVMuxer_Create failed");
        return false;
    }
    // add video track
    int32_t videoTrackId{-1};
    OH_AVFormat *videoFormat = OH_AVFormat_Create();
    if (videoFormat == nullptr) {
        OH_LOG_ERROR(LOG_APP, "OH_AVFormat_Create failed");
        return false;
    }
    OH_AVFormat_SetIntValue(videoFormat, OH_MD_KEY_WIDTH, width);
    OH_AVFormat_SetIntValue(videoFormat, OH_MD_KEY_HEIGHT, height);
    OH_AVFormat_SetStringValue(videoFormat, OH_MD_KEY_CODEC_MIME, OH_AVCODEC_MIMETYPE_VIDEO_AVC);
    int ret = OH_AVMuxer_AddTrack(muxer_, &videoTrackId, videoFormat);
    OH_AVFormat_Destroy(videoFormat);
    if (ret != AV_ERR_OK || videoTrackId < 0) {
        OH_LOG_ERROR(LOG_APP, "OH_AVMuxer_AddTrack failed");
        return false;
    }
    encInfo_.videoTrackId = videoTrackId;
    return true;
}
bool PicturesEncoder::InitEncoder(int32_t width, int32_t height, int32_t pixelFormat)
{
    OH_AVCapability *capability = OH_AVCodec_GetCapability(OH_AVCODEC_MIMETYPE_VIDEO_AVC, true);
    if (capability == nullptr) {
        OH_LOG_ERROR(LOG_APP, "OH_AVCodec_GetCapability failed");
        return false;
    }
    bool isSupported = OH_AVCapability_IsVideoSizeSupported(capability, width, width);
    if (!isSupported) {
        OH_LOG_ERROR(LOG_APP, "OH_AVCapability_IsVideoSizeSupported false");
        return false;
    }
    videoEnc_ = OH_VideoEncoder_CreateByMime(OH_AVCODEC_MIMETYPE_VIDEO_AVC);
    if (videoEnc_ == NULL) {
        OH_LOG_ERROR(LOG_APP, "OH_VideoEncoder_CreateByMime failed");
        return false;
    }
    int32_t ret{0};
    OH_AVCodecCallback encoderCB = {&OnError, &OnStreamChanged, &OnNeedInputBuffer, &OnNewOutputBuffer};
    ret = OH_VideoEncoder_RegisterCallback(videoEnc_, encoderCB, &userData_);
    if (ret != AV_ERR_OK) {
        OH_LOG_ERROR(LOG_APP, "OH_VideoEncoder_RegisterCallback failed");
        return false;
    }
    OH_AVFormat *videoFormat = OH_AVFormat_Create();
    if (videoFormat == nullptr) {
        OH_LOG_ERROR(LOG_APP, "OH_AVFormat_Create failed");
        return false;
    }
    OH_AVFormat_SetIntValue(videoFormat, OH_MD_KEY_WIDTH, width);
    OH_AVFormat_SetIntValue(videoFormat, OH_MD_KEY_HEIGHT, height);
    OH_AVFormat_SetIntValue(videoFormat, OH_MD_KEY_PIXEL_FORMAT, pixelFormat);
    ret = OH_VideoEncoder_Configure(videoEnc_, videoFormat);
    OH_AVFormat_Destroy(videoFormat);
    if (ret != AV_ERR_OK) {
        OH_LOG_ERROR(LOG_APP, "OH_VideoEncode_Configure failed");
        return false;
    }
    return true;
}
bool PicturesEncoder::StartMuxer()
{
    assert(muxer_ != nullptr);
    int32_t ret{AV_ERR_OK};
    ret = OH_AVMuxer_Start(muxer_);
    if (ret != AV_ERR_OK) {
        OH_LOG_ERROR(LOG_APP, "OH_AVMuxer_Start failed");
        return false;
    }
    return true;
}
bool PicturesEncoder::StartEncoder()
{
    assert(videoEnc_ != nullptr);
    int32_t ret{AV_ERR_OK};
    ret = OH_VideoEncoder_Prepare(videoEnc_);
    if (ret != AV_ERR_OK) {
        OH_LOG_ERROR(LOG_APP, "OH_VideoEncoder_Prepare failed");
        return false;
    }
    ret = OH_VideoEncoder_Start(videoEnc_);
    if (ret != AV_ERR_OK) {
        OH_LOG_ERROR(LOG_APP, "OH_VideoEncoder_Start failed");
        return false;
    }
    return true;
}
bool PicturesEncoder::Init(std::string path, int32_t width, int32_t height)
{
    std::unique_lock<std::mutex> lock(mutex_);
    (void)lock;
    encInfo_.filePath = path;
    encInfo_.vidWidth = width;
    encInfo_.vidHeight = height;
    if (isStart_) {
        OH_LOG_ERROR(LOG_APP, "PicturesEncoder is running");
        return false;
    }
    assert(muxer_ == nullptr);
    assert(videoEnc_ == nullptr);
    bool success{false};
    success = InitMuxer(encInfo_.filePath, encInfo_.vidWidth, encInfo_.vidHeight);
    if (!success) {
        OH_LOG_ERROR(LOG_APP, "failed to initialize muxer");
        return false;
    }
    success = InitEncoder(encInfo_.vidWidth, encInfo_.vidHeight, encInfo_.pixelFormat);
    if (!success) {
        OH_LOG_ERROR(LOG_APP, "failed to initialize video encoder");
        return false;
    }
    encInfo_.pts = 0;
    return true;
}
bool PicturesEncoder::Start()
{
    std::unique_lock<std::mutex> lock(mutex_);
    (void)lock;
    if (isStart_) {
        OH_LOG_ERROR(LOG_APP, "PicturesEncoder is running");
        return false;
    }
    assert(inputThread_.get() == nullptr);
    assert(outputThread_.get() == nullptr);
    isStart_.store(true);
    isFirstFrame.store(true);
    if (releaseThread_ && releaseThread_->joinable()) {
        releaseThread_->join();
        releaseThread_.reset();
        releaseThread_ = nullptr;
    }
    bool success{false};
    success = StartMuxer();
    if (!success) {
        OH_LOG_ERROR(LOG_APP, "failed to start muxer");
        return false;
    }
    success = StartEncoder();
    if (!success) {
        OH_LOG_ERROR(LOG_APP, "failed to start video encoder");
        return false;
    }
    inputThread_ = std::make_unique<std::thread>(&PicturesEncoder::VideoEncInput, this);
    outputThread_ = std::make_unique<std::thread>(&PicturesEncoder::VideoEncOutput, this);
    if (inputThread_ == nullptr || outputThread_ == nullptr) {
        OH_LOG_ERROR(LOG_APP, "create encoder thread failed");
        return false;
    }
    return true;
}