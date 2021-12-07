#pragma once

#include <iostream>

#include <GL/gl3w.h>

namespace Render::GL{
    bool load(){
        if(gl3wInit()){
            std::cerr << "Failed to initialize OpenGL (gl3w)\n";
            return false;
        }
        if(!gl3wIsSupported(4,6)){
            std::cerr << "GL 4.6 isn't supported!\n";
            return false;
        }
        return true;
    }
}