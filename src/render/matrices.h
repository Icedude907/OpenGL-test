#pragma once

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

// Object structure:
    // Local coordinate
    // - (work up through parents)
    // World coordinate
// Rendering
    // View matrix
    // Projection matrix
    // Clipping

inline glm::mat4 makeMatrix(){
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4  view = glm::mat4(1.0f);

    view = glm::translate(view, glm::vec3(0.0f, 0.5f, -2.0f)); // Shift the world by this
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)1920 / 1080, 0.1f, 100.0f);
}