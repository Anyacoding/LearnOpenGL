//
// Created by anya on 2023/11/3.
//

#ifndef LEARNOPENGL_POINTLIGHT_HPP
#define LEARNOPENGL_POINTLIGHT_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace anya {

    struct PointLight {
        glm::vec3 position;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;

        float constant;
        float linear;
        float quadratic;

    };

}

#endif //LEARNOPENGL_POINTLIGHT_HPP
