// Obligatory OpenGL 2D platformer I made to put basic concepts into practice
// Doesn't work on MacOS since its restricted to 3.3
//

// I wish there were better bindings for OGL and C++, but what can you do.
// OpenGL is no longer being updated, with all the things moving tom VK.

#include <cmath>
#include <string>
#include <utility>
#include <iostream>
#include <thread>

#include <incbin.h>

#include <GL/gl3w.h>
#define VKFW_INLINE
#define VKFW_NO_INCLUDE_VULKAN_HPP
// #define VKFW_NO_EXCEPTIONS
#define VKFW_NO_LEADING_e_IN_ENUMS
#include <vkfw/vkfw.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef _WIN32
    #define OS_WINDOWS
    #include <Windows.h>
#endif

#include <stb_image.h>

//
// Start my stuff
//
#ifndef NDEBUG
    #define DEBUG_SHADERS
#endif

#include "render/render.h"
#include "render/screen.h"
#include "util/util.h"


void error_callback(int error_code, const char* description){
    std::cerr << "Error: " << description << std::endl;
}

enum ExitCodes: int{
    OK = 0,
    ERR = -1,
    WINDOW_CREATION_ERROR = 0x10,
    OPENGL_INITIALISATION_ERROR = 0x20,
    OPENGL_UNSUPPORTED_VERSION = 0x21,
    DATA_LOAD_FAILED = 0x30,
};

extern "C"{
    #undef INCBIN_PREFIX
    #define INCBIN_PREFIX r_
    INCBIN(char, basicVertexFile, "basic.vsh");
    INCBIN(char, basicFragmentFile, "basic.fsh");
}

struct ShaderData{
    glm::vec3 pos;
    glm::vec3 col;
    glm::vec2 uv;
};
// using ShaderData = std::tuple<glm::vec3, glm::vec3, glm::vec2>;
// Broked because padding
ShaderData shaderData[] = { // positions, colours, uv maps
    // {{-0.5f,    -0.5f * float(sqrt(3)) / 3,     0.0f}, /* Outer left  */ {0.0f, 1.0f , 0.0f }, {0.0f, 0.0f}},
    // {{ 0.5f,    -0.5f * float(sqrt(3)) / 3,     0.0f}, /* Outer right */ {0.0f, 0.0f , 1.0f }, {1.0f, 0.0f}},
    // {{ 0.0f,     0.5f * float(sqrt(3)) * 2 / 3, 0.0f}, /* Outer top   */ {1.0f, 0.0f , 0.0f }, {0.5f, 1.0f}},
    // {{-0.5f / 2, 0.5f * float(sqrt(3)) / 6,     0.0f}, /* Inner left  */ {0.9f, 0.45f, 0.17f}, {0.0f, 0.0f}},
    // {{ 0.5f / 2, 0.5f * float(sqrt(3)) / 6,     0.0f}, /* Inner right */ {0.0f, 0.0f , 0.0f }, {0.0f, 0.0f}},
    // {{ 0.0f,    -0.5f * float(sqrt(3)) / 3,     0.0f}, /* Inner down  */ {0.5f, 0.5f , 0.5f }, {0.0f, 0.0f}},   
    {{-0.5f, 0.0f,  0.5f}, {0.0f, 1.0f , 0.0f }, {0.0f, 0.0f,}},
    {{-0.5f, 0.0f, -0.5f}, {0.0f, 0.0f , 1.0f }, {5.0f, 0.0f,}},
    {{ 0.5f, 0.0f, -0.5f}, {1.0f, 0.0f , 0.0f }, {0.0f, 0.0f,}},
    {{ 0.5f, 0.0f,  0.5f}, {0.9f, 0.45f, 0.17f}, {5.0f, 0.0f,}},
    {{ 0.0f, 0.8f,  0.0f}, {0.0f, 0.0f , 0.0f }, {2.5f, 5.0f }},
};
glm::vec<3, GLuint> indices[] = { // Points to use to make a tringle
    // {0, 3, 5}, // Lower left triangle
    // {3, 2, 4}, // Lower right triangle
    // {5, 4, 1} // Upper triangle
    // {0,1,2} // tringle
    {0, 1, 2}, // pyramid
	{0, 2, 3}, // pyramid
	{0, 1, 4}, // pyramid
	{1, 2, 4}, // pyramid
	{2, 3, 4}, // pyramid
	{3, 0, 4}, // pyramid
};

using namespace Render; // TODO: sub in
class Instance{
    public:

    private: // Things are deconstructed from top to bottom
    VertexArrays VAO;
    VertexBuffers VBO;
    ElementBuffers EBO;
    Textures2D obamium;
    ShaderProgram shaderProgram;
        ShaderUniform colourScale, tex0, Umodel, Uview, Uproj;

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4  view = glm::mat4(1.0f);
    glm::mat4  proj = glm::mat4(1.0f);
    float nearplane = 0.1f, farplane = 100.0f;

    Screen screen;

    public:
    int run(){
        auto res = setup();
        if(res != 0) return res;
        while(!screen.window->shouldClose()){
            draw();
            vkfw::pollEvents();
        }
        stop();
        return 0;
    }

    int setup(){
        if(screen.init("Obligatory platformer", 852, 480, error_callback) == false){
            return ExitCodes::WINDOW_CREATION_ERROR;
        }

        // Render empty frame.
        glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        screen.present();

        VertexShader vertexShader(r_basicVertexFileData, r_basicVertexFileSize);
        FragmentShader fragmentShader(r_basicFragmentFileData, r_basicFragmentFileSize);
        shaderProgram = ShaderProgram(vertexShader, fragmentShader);
        vertexShader.Delete();
        fragmentShader.Delete();
        
        VAO = VertexArrays(1);
        VBO = VertexBuffers(1);
        EBO = ElementBuffers(1);
        VAO.use();
        VBO.use();
        VertexBuffers::setVertexBufferData(sizeof(shaderData), shaderData);
        EBO.use();
        ElementBuffers::setElementBufferData(sizeof(indices), indices);

        // Configures vertex attributes
        // Modify attribute 0, each vertex is 3d, floating point, not normalised, step 1 vec at a time (you could drop this for interesting effects), start offset 0
        constexpr GLsizeiptr stride = sizeof(ShaderData);
        Util::PostfixAccumulator<size_t> offset = 0;
        VAO.LinkBuffers(VBO, 0, 3, AttribPtr::FLOAT, stride, offset.postfixAdd(sizeof(glm::vec3)));
        VAO.LinkBuffers(VBO, 1, 3, AttribPtr::FLOAT, stride, offset.postfixAdd(sizeof(glm::vec3)));
        VAO.LinkBuffers(VBO, 2, 2, AttribPtr::FLOAT, stride, offset.postfixAdd(sizeof(glm::vec2)));
            // Initially I thought float based rendering was a moot point
            // Actually could be useful when applying effects and temp precision before finalising (maybe half floats are even cooler?)
            // Also HDR rendering requires a luminance value so beyond strict rgb can help with that
        
        // Just done to prevent acidentally modifying them
        VertexBuffers::unbind();
        VertexArrays::unbind();
        ElementBuffers::unbind();


        // Texture
        int widthImg, heightImg, channelsImg;
        uint8_t* textureData = stbi_load("resources/Obamium.jpg", &widthImg, &heightImg, &channelsImg, 0);
        if(textureData == nullptr){
            std::cout << stbi_failure_reason() << "\n"
            << "Make sure you are running from the folder above `resources!`" << std::endl;
            return ExitCodes::DATA_LOAD_FAILED;
        }
        obamium = Textures2D(1);
        obamium.activate(0);
        obamium.use();
        Textures2D::setParamI(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        Textures2D::setParamI(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // Textures2D::setParamI(GL_TEXTURE_WRAP_S, GL_REPEAT);
        // Textures2D::setParamI(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        //  // Extra lines in case you choose to use GL_CLAMP_TO_BORDER
        // float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

        // 1st `0` is mipmap levels, second `0` is legacy.
        Textures2D::setTexture2D(0, GL_RGB, widthImg, heightImg, GL_UNSIGNED_BYTE, textureData);
        Textures2D::generateMipmaps();
        stbi_image_free(textureData);
        Textures2D::unbind();

        colourScale = shaderProgram.getUniform("colourScale");
        tex0 = shaderProgram.getUniform("tex0");
        Umodel = shaderProgram.getUniform("model");
        Uview = shaderProgram.getUniform("view");
        Uproj = shaderProgram.getUniform("proj");

        view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f)); // Shift the world by this
        proj = glm::perspective(glm::radians(45.0f), (float)screen.width / screen.height, nearplane, farplane);

        shaderProgram.use();
        tex0.setUniform(0); // Sampler2D index 0 Corresponds to activated texture 0 and so on.
        Umodel.setUniform(model);
        Uview.setUniform(view);
        Uproj.setUniform(proj);

        screen.frameTimes.start();

        screen.window->callbacks()->on_framebuffer_resize = std::bind(resize_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        return 0;
    }

    void draw(){
        if(!screen.shouldRender){ std::this_thread::sleep_for(std::chrono::milliseconds(1000/60)); }
        screen.frameTimes.update();
        glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderProgram.use();
        colourScale.setUniform(1.5f);
        // model, rotation amount (90deg/s), axis weighting
        model = glm::rotate(model, (float)glm::radians(90*screen.frameTimes.delta), glm::vec3(0.0f, 1.0f, 0.0f));
        // model = glm::rotate(model, (float)glm::radians(180*presentTime.delta), glm::vec3(0.0f, 0.0f, 1.0f));
        Umodel.setUniform(model);
        obamium.use();
        VAO.use();
        // 4th arg is a pointer to indices if VBOs aren't being used (but they are so we keep it null)
        // Every time this is used without a VBO, all the indices have to be reuploaded to the gpu.
        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(GLuint), GL_UNSIGNED_INT, nullptr);
        screen.present();
    }

    void stop(){
        VBO.Delete();
        VAO.Delete();
        EBO.Delete();
        obamium.Delete();
        shaderProgram.Delete();
    }

    //
    //
    //

    void resize_callback(const vkfw::Window& window, size_t width, size_t height){
        screen.resize(width, height);
        if(screen.shouldRender){
            // recompute projection matrix
            proj = glm::perspective(glm::radians(45.0f), (float)width / height, nearplane, farplane);
            Uproj.setUniform(proj);
            draw();
        }
    }

    ~Instance(){ }

};

int main(){
    #ifdef OS_WINDOWS
        SetProcessDPIAware();
    #endif
    try{
        Instance instance;
        auto res = instance.run();
        if(res != 0) return res;
    }catch (std::system_error &err) {
        std::cerr << "Something messed up: " << err.what() << std::endl;
        return ExitCodes::ERR;
    }
    return ExitCodes::OK;
}