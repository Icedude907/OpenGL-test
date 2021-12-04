#pragma once

#include <iostream>
#include <functional>

#include <vkfw/vkfw.hpp>

#include <glm/glm.hpp>

#include "input.h"
#include "matrices.h"

/*
    Basic camera bound to glfw for moving around
*/
struct Camera{
    glm::vec3 pos = glm::vec3(0.0f, 0.5f, 2.0f);
    glm::vec3 rot = glm::vec3(0.f, 0.f, 0.f);
    constexpr static glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);

    float fovrad = glm::radians(45.f);
    float nearplane = 0.01f;
    float farplane = 100.0f;

    float speed = 2.0f; // units/s
    float sensitivity = 100.0f;

    inline Camera(){
    }

    inline void setfov(float degrees){
        fovrad = glm::radians(degrees);
    }

    // from world space -> camera space -> screen space
    glm::mat4 generateViewProjectionMatrix(size_t width, size_t height){
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 proj = glm::mat4(1.0f);
        view = Render::Matrix::lookIn(pos, orientation, glm::vec3(0.f, 1.f, 0.f)); // TODO: I DONT LIKE THIS BUT ILL EDIT IT ONCE I GET THINGS WORKING
            view = glm::rotate(view, rot.x, glm::vec3(1.f, 0.f, 0.f));
            view = glm::rotate(view, rot.y, glm::vec3(0.f, 1.f, 0.f));
            view = glm::rotate(view, rot.z, glm::vec3(0.f, 0.f, 1.f));
        proj = glm::perspective(fovrad, (float)width / height, nearplane, farplane);
        glm::mat4 projview = proj * view;
        return projview;
    }

    constexpr inline void resetRotation(){
        rot = glm::vec3(0.f, 0.f, 0.f);
    }
};