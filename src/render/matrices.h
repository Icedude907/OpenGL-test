#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Object structure:
    // Local coordinate
    // - (work up through parents)
    // World coordinate
// Rendering
    // View matrix
    // Projection matrix
    // Clipping

namespace Render::Matrix{

    // Adapted from glm::lookAtRH
    template<typename T, glm::qualifier Q>
    GLM_FUNC_QUALIFIER glm::mat<4, 4, T, Q> lookIn(glm::vec<3, T, Q> const& eye, glm::vec<3, T, Q> const& direction, glm::vec<3, T, Q> const& up){
        using namespace glm;
		vec<3, T, Q> const f(normalize(direction));
		vec<3, T, Q> const s(normalize(cross(f, up)));
		vec<3, T, Q> const u(cross(s, f));

		mat<4, 4, T, Q> Result(1);
		Result[0][0] = s.x;
		Result[1][0] = s.y;
		Result[2][0] = s.z;
		Result[0][1] = u.x;
		Result[1][1] = u.y;
		Result[2][1] = u.z;
		Result[0][2] =-f.x;
		Result[1][2] =-f.y;
		Result[2][2] =-f.z;
		Result[3][0] =-dot(s, eye);
		Result[3][1] =-dot(u, eye);
		Result[3][2] = dot(f, eye);
		return Result;
    }
}