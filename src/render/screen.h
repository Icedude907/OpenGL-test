#pragma once

#include <vkfw/vkfw.hpp>
#include "../util/util.h"
#include "frametimer.h"

// Interfaces with gl/vkfw
namespace Render{
    struct Screen{
        size_t width, height;
        bool shouldRender;

        vkfw::UniqueInstance glfwInstance;
        vkfw::UniqueWindow window;

        Render::FrameTimer frameTimes;

        bool init(char const* title, size_t width, size_t height, GLFWerrorfun error_callback = nullptr){
            this->width = width;
            this->height = height;
            glfwInstance = vkfw::initUnique(); // Unique automatically deallocates
            vkfw::WindowHints hints;
                hints.clientAPI = vkfw::ClientAPI::OpenGL;
                hints.contextVersionMajor = 4;
                hints.contextVersionMinor = 6;
                hints.openGLProfile = vkfw::OpenGLProfile::Core;
            window = vkfw::createWindowUnique(width, height, title, hints);
            vkfw::setErrorCallback(error_callback);
            if(!window){
                std::cout << "Window Creation Failed!" << std::endl;
                return false;
            }
            window->makeContextCurrent();

            if(gl3wInit()){
                std::cerr << "Failed to initialize OpenGL (gl3w)\n";
                return false;
            }
            if(!gl3wIsSupported(4,6)){
                std::cerr << "GL 4.6 isn't supported!\n";
                return false;
            }
            resizeViewport();
            updateShouldRender();
            return true;
        }
        inline void resize(size_t newWidth, size_t newHeight){
            width = newWidth;
            height = newHeight;
            resizeViewport();
            updateShouldRender();
        }
        inline void present(){
            window->swapBuffers();
        }
        private:
        inline void updateShouldRender(){ shouldRender = !(width == 0 || height == 0); }
        inline void resizeViewport(){
            glViewport(0, 0, this->width, this->height);
        }
        
    };
}