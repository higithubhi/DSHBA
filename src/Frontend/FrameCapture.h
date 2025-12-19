#pragma once

#include <string>
#include <chrono>
#include <memory>
#include <queue>
#include <mutex>
// 帧捕获器类 - 用于保存每帧画面为PNG图片
class FrameCapture {
public:
    FrameCapture();
    ~FrameCapture();
    
    // 启用/禁用帧捕获
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
    // 设置输出目录
    void setOutputDirectory(const std::string& directory);
    
    // 捕获当前帧
    void captureFrame(int window_width, int window_height);
    
    // 获取已捕获的帧数
    size_t getCaptureCount() const;
    
    // 设置最大捕获帧数（0表示无限制）
    void setMaxFrames(size_t max_frames);    
    void setSkipFrames(size_t skip){
        skip_frames=skip;
    }
    
private:
    bool enabled_;
    std::string output_directory_;
    size_t capture_count_;
    size_t max_frames_;
    size_t skip_frames;
    
    int width;
    int height;
    std::mutex _framesMutex;
    std::queue<std::unique_ptr<std::vector<uint8_t>>> frames;
    std::unique_ptr<std::thread> enThread;
    
    // 生成带时间戳的文件名
    std::string generateFilename();
    
    // 保存OpenGL帧缓冲为PNG
    bool saveFrameAsPNG(std::vector<uint8_t>& pixel);
    
    // 获取当前时间戳字符串
    std::string getCurrentTimestamp();
    
    // 创建输出目录（如果不存在）
    bool ensureOutputDirectory();
};