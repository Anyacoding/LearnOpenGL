//
// Created by anya on 2023/11/6.
//

#ifndef LEARNOPENGL_DIRECTIONLIGHT_HPP
#define LEARNOPENGL_DIRECTIONLIGHT_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace anya {

    struct DirLight {
        glm::vec3 direction;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };

}

#endif //LEARNOPENGL_DIRECTIONLIGHT_HPP
