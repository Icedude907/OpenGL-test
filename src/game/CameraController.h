#pragma once

#include <vkfw/vkfw.hpp>

#include "../render/camera.h"
#include "../input/input.h"

namespace Game{
    struct CameraController{
        Camera& camera;
        glm::vec3 camMoveVec = glm::vec3(0.f,0.f,0.f);
        glm::vec3 camRotVec = glm::vec3(0.f,0.f,0.f);
        float speed = 2.0f; // units/s
        float sensitivity = 100.0f;

        inline CameraController(Camera& camera): camera(camera){}

        constexpr static Input::ButtonKey
            keyEsc{vkfw::Key::Escape},
            keyR{vkfw::Key::R},
            keyLc{vkfw::MouseButton::Left},
            keyW{vkfw::Key::W},
            keyS{vkfw::Key::S},
            keyA{vkfw::Key::A},
            keyD{vkfw::Key::D},
            keySp{vkfw::Key::Space},
            keySh{vkfw::Key::LeftShift},
            keyQ{vkfw::Key::Q},
            keyE{vkfw::Key::E};
        void onCameraKeyEvent(Input::Action::Button e){
            // X Y Z
            using namespace Input::Actions;
            static bool forward, back, left, right, up, down = false;
            static bool pitchL, pitchR = false;
            static bool captureMouse = false;
            if(e.matches(keyR) ){ camera.resetRotation(); }
            if(e.matches(keyEsc, PRESS)){ captureMouse = false; }
            if(e.matches(keyLc,  PRESS)){ captureMouse = true; }

            if(e.matches(keyW) ){ forward = e.pressed(); }
            if(e.matches(keyS) ){    back = e.pressed(); }
            if(e.matches(keyA) ){    left = e.pressed(); }
            if(e.matches(keyD) ){   right = e.pressed(); }
            if(e.matches(keySp)){      up = e.pressed(); }
            if(e.matches(keySh)){    down = e.pressed(); }
            if(e.matches(keyQ) ){  pitchL = e.pressed(); }
            if(e.matches(keyE) ){  pitchR = e.pressed(); }
            auto computeCamMovVec = [&](){
                glm::vec3 mov = glm::vec3(0.0f, 0.0f, 0.0f);
                glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f);
                if(right^left)   { mov.x = speed*(right-left); }
                if(up^down)      { mov.y = speed*(up-down); }
                if(forward^back) { mov.z = speed*(back-forward); } // -z is into page
                if(pitchL^pitchR){ rot.z = speed*(pitchL-pitchR); }

                camMoveVec = mov;
                camRotVec = rot;
            };
            computeCamMovVec();
        };
        void setupCameraInput(){
            using namespace Input;
            using namespace Input::Actions;
            ButtonInputHandler handle{
                std::bind(onCameraKeyEvent, this, std::placeholders::_1),
                [](ActiveContexts& contexts)->bool{return true;},
                {
                    {keyEsc, PRESS|RELEASE},
                    {keyR, PRESS|RELEASE},
                    {keyLc, PRESS|RELEASE},
                    {keyW, PRESS|RELEASE},
                    {keyS, PRESS|RELEASE},
                    {keyA, PRESS|RELEASE},
                    {keyD, PRESS|RELEASE},
                    {keySp, PRESS|RELEASE},
                    {keySh, PRESS|RELEASE},
                    {keyQ, PRESS|RELEASE},
                    {keyE, PRESS|RELEASE},
                }
            };
            inputSystem.addButtonHandler(handle);
        }
    };
}