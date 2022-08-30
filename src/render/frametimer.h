#pragma once
#include <chrono>
namespace Render{
    struct FrameTimer{
        std::chrono::steady_clock::time_point lastFrameTime; // Supposedly steady_clock is immune to system time changes
        double delta; // In seconds
        inline void start(){
            lastFrameTime = std::chrono::steady_clock::now();
            delta = 0.0f;
        }
        inline void nextFrame(){
            auto time = std::chrono::steady_clock::now();
            delta = std::chrono::duration<double>(time - lastFrameTime).count();
            lastFrameTime = time;
        }
    };
};