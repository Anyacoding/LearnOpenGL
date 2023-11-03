//
// Created by anya on 2023/11/3.
//

#ifndef LEARNOPENGL_SPOTLIGHT_HPP
#define LEARNOPENGL_SPOTLIGHT_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace anya {

    struct SpotLight {
        glm::vec3 position;
        glm::vec3 direction;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;

        float cutOff;
        float outerCutOff;
    };

}

#endif //LEARNOPENGL_SPOTLIGHT_HPP
