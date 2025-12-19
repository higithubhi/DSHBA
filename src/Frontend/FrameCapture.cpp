#include "FrameCapture.h"
#include <GLES3/gl32.h>
#include <SDL.h>
#include <fstream>
#include <hilog/log.h>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <iostream>
#include <thread>

#ifdef _WIN32
#include <direct.h>
#define mkdir _mkdir
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

FrameCapture::FrameCapture() 
    : enabled_(false)
    , output_directory_("frames")
    , capture_count_(0)
    , max_frames_(0) 
    , skip_frames(0)
{
}

FrameCapture::~FrameCapture() {
    setEnabled(false);
}

void FrameCapture::setEnabled(bool enabled) {
    if (enabled && !enabled_) {
        std::cout << "Frame capture enabled. Output directory: " << output_directory_ << std::endl;
        if (!ensureOutputDirectory()) {
            std::cerr << "Failed to create output directory: " << output_directory_ << std::endl;
            return;
        }
    } else if (!enabled && enabled_) {
        std::cout << "Frame capture disabled. Captured " << capture_count_ << " frames." << std::endl;
    }
    enabled_ = enabled;
    if(!enThread&&enabled_){
        enThread=std::make_unique<std::thread>([&]{
            while(enabled_)
            {
                if(frames.empty()){
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
                else{
                    auto& pic=frames.front();
                    saveFrameAsPNG(*pic);
                    {
                        std::lock_guard<std::mutex> lock(_framesMutex);
                        frames.pop();
                    }
                }
            }
        });
    }    
}

bool FrameCapture::isEnabled() const {
    return enabled_;
}

void FrameCapture::setOutputDirectory(const std::string& directory) {
    output_directory_ = directory;
}

size_t FrameCapture::getCaptureCount() const {
    return capture_count_;
}

void FrameCapture::setMaxFrames(size_t max_frames) {
    max_frames_ = max_frames;
}

std::string FrameCapture::generateFilename() {
    std::stringstream ss;
    ss << "frame_" << std::setw(6) << std::setfill('0') << capture_count_ 
       << "_" << getCurrentTimestamp() << ".png";
    return output_directory_ + "/" + ss.str();
}

std::string FrameCapture::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
    ss << "_" << std::setw(3) << std::setfill('0') << ms.count();
    return ss.str();
}

bool FrameCapture::ensureOutputDirectory() {
    try {
        if (!std::filesystem::exists(output_directory_)) {
            return std::filesystem::create_directories(output_directory_);
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        return false;
    }
}


#include <multimedia/image_framework/image/pixelmap_native.h>
#include <multimedia/image_framework/image_pixel_map_mdk.h>
#include <multimedia/image_framework/image/image_packer_native.h>
#include <fcntl.h>
#include <unistd.h>
bool FrameCapture::saveFrameAsPNG(std::vector<uint8_t>& pixel)
{
    // 1. 读取OpenGL RGBA像素数据    
    static Image_MimeType mimeType = {"image/png",9};    
    static OH_Pixelmap_InitializationOptions* pixeloptions=nullptr;
    static OH_PackingOptions* options=nullptr;
    static OH_ImagePackerNative* packer=nullptr;
    if(pixeloptions==nullptr){
        OH_PixelmapInitializationOptions_Create(&pixeloptions);
        OH_PixelmapInitializationOptions_SetWidth(pixeloptions,width);
        OH_PixelmapInitializationOptions_SetHeight(pixeloptions,height);
        OH_PixelmapInitializationOptions_SetSrcPixelFormat(pixeloptions,PIXEL_FORMAT_RGBA_8888);
    }
    if(options==nullptr){
        OH_PackingOptions_Create(&options);   
        OH_PackingOptions_SetMimeType(options, &mimeType);
    }
    if(packer==nullptr){
        OH_ImagePackerNative_Create(&packer);
    }

    OH_PixelmapNative* pixelmap;    
    auto errCode = OH_PixelmapNative_CreatePixelmap(pixel.data(),pixel.size(),pixeloptions,&pixelmap);
    if (errCode != IMAGE_SUCCESS) {        
        OH_LOG_ERROR(LOG_APP, "ImagePackerNativeCTest OH_PixelmapNative_CreatePixelmap  failed, errCode: %{public}d.", errCode);
        return false;
    }
    std::string filename=generateFilename();
    int32_t fd = open(filename.c_str(), O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR|S_IROTH|S_IWOTH);    
    errCode = OH_ImagePackerNative_PackToFileFromPixelmap(packer, options, pixelmap, fd);
    if (errCode != IMAGE_SUCCESS) {
        OH_LOG_ERROR(LOG_APP, "ImagePackerNativeCTest OH_ImagePackerNative_PackToFileFromPixelmap  failed, errCode: %{public}d.", errCode);
    }
    close(fd);
    OH_PixelmapNative_Release(pixelmap);
//    OH_ImagePackerNative_Release(packer);
//    OH_PixelmapInitializationOptions_Release(pixeloptions);
//    OH_PackingOptions_Release(options);    
    return true;
}

void FrameCapture::captureFrame(int window_width, int window_height) {
    if (!enabled_) {
        return;
    }
    
    // 检查最大帧数限制
    if (max_frames_ > 0 && capture_count_ >= max_frames_) {
        return;
    }
    capture_count_++;
    if(capture_count_<skip_frames){        
        return;
    }
    
    width=window_width;
    height=window_height;    
    
    auto pixels=std::make_unique<std::vector<uint8_t>>(width*height*4);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels->data());
    {
        std::lock_guard<std::mutex> lock(_framesMutex);
        frames.push(std::move(pixels));    
    }    
}