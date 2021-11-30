#include "render.h"

#include <iostream>

namespace Render{
    ShaderProgram::ShaderProgram(VertexShader vertex, FragmentShader fragment){
        program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
        #ifndef DEBUG_SHADERS
            printCompilerErrors();
        #endif
    }
}