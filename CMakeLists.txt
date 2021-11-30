cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(ObligatoryPlatformer VERSION 1.0)
set(glfw_version 3.3.5)
set(vkfw_timestamp 21-1-21)
set(glm_version 0.9.9.8)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# setting up variables
set(glfw_name "glfw-${glfw_version}")
set(vkfw_name "vkfw@${vkfw_timestamp}")
set(glm_name "glm-${glm_version}")

# building

# TODO: Disable tests etc
add_subdirectory("libs/${glfw_name}")
add_subdirectory("libs/gl3w")
add_subdirectory("libs/${glm_name}")

add_executable(OPGL "src/main.cpp" "src/render/render.cpp" "src/util/stbinit.cpp")
target_include_directories(OPGL PUBLIC 
    # TODO: is including subdirectories required
    "libs/${glfw_name}/include"
    "libs/${vkfw_name}/include"
    "libs/gl3w/include"
    "libs/${glm_name}"
    "libs/incbin"
    "libs/stb"
    "src/shader"
)
target_link_libraries(OPGL PUBLIC opengl32 gl3w glfw ${GLFW_LIBRARIES})