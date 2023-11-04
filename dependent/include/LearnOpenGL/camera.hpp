//
// Created by Anya on 2023/10/26.
//

#ifndef LEARNOPENGL_CAMERA_HPP
#define LEARNOPENGL_CAMERA_HPP

#include <array>

namespace anya {

    class Camera {
    public:
        // 相机三要素
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;

        // 视野
        float fov = 45.0f;

        // 欧拉角
        float yaw = -90.0f;
        float pitch = 0.0f;

        // 相机速度
        float speed = 4.5f;

        // 开启手电
        bool isOpenSpotLight = false;
    public:
        Camera(const glm::vec3& cameraPos,
               const glm::vec3& cameraFront,
               const glm::vec3& cameraUp) :
                position(cameraPos), front(cameraFront), up(cameraUp) {

        }

    public:
        [[nodiscard]] glm::mat4
        getViewMatrix() const {
            return glm::lookAt(position, position + front, up);
        }

        [[nodiscard]] glm::mat4
        getProjectionMatrix() const {
            return glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
        }
    };

}



#endif //LEARNOPENGL_CAMERA_HPP
