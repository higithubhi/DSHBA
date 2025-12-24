#include <glad/glad.h>
#include <SDL.h>
#include <SDL_image.h>
#include <fstream>
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
#include "FrameCapture.h"

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


bool FrameCapture::saveFrameAsPNG(std::vector<uint8_t>& pixel)
{
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
        pixel.data(),
        width,
        height,
        32,
        width * 4,
        0x000000FF,
        0x0000FF00,
        0x00FF0000,
        0xFF000000
    );
    IMG_SavePNG(surface, generateFilename().c_str());
    SDL_FreeSurface(surface);
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