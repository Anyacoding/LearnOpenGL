//
// Created by Anya on 2023/10/26.
//

#ifndef LEARNOPENGL_IODEVICE_HPP
#define LEARNOPENGL_IODEVICE_HPP

class Device {
public:
    // 第一次使用鼠标
    bool firstMouse = true;
    // 鼠标灵敏度
    float mouseSensitivity = 0.05f;
    // 按键映射
    std::array<bool, 1024> keys = {};
    // 鼠标的起始位置
    float lastX = 400, lastY = 300;
};

#endif //LEARNOPENGL_IODEVICE_HPP
