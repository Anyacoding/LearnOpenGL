//
// Created by Anya on 2023/10/31.
//

#ifndef LEARNOPENGL_VERTEX_HPP
#define LEARNOPENGL_VERTEX_HPP

#include <glm/glm.hpp>

namespace anya {

    struct Vertex {
        glm::vec3  position;
        glm::vec3 normal;
        glm::vec2 texCoords;
    };

}

#endif //LEARNOPENGL_VERTEX_HPP
