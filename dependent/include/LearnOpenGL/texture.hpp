//
// Created by Anya on 2023/10/28.
//

#ifndef LEARNOPENGL_TEXTURE_HPP
#define LEARNOPENGL_TEXTURE_HPP

#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#include <glm/glm.hpp>

class Texture {
private:
    int width = 800, height = 600, nrChannels = 3;
    GLint format = GL_RGB;
    unsigned int textureID = -1;

public:
    explicit Texture(const std::string& path) {
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(path.c_str(), &this->width, &this->height, &this->nrChannels, 0);

        if (data == nullptr) {
            std::cerr << "ERROR IMAGE NOT FOUND" << std::endl;
            stbi_image_free(data);
            return;
        }

        switch (this->nrChannels) {
            case 1: {
                this->format = GL_RED;
                break;
            }
            case 3: {
                this->format = GL_RGB;
                break;
            }
            case 4: {
                this->format = GL_RGBA;
                break;
            }
        }

        glGenTextures(1, &this->textureID);
        glBindTexture(GL_TEXTURE_2D, this->textureID);

        // 为当前绑定的纹理对象设置环绕、过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // 载入纹理src并生成mipmap, 注意此图的通道是RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, this->format, this->width, this->height, 0, this->format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // 释放纹理src
        stbi_image_free(data);
    }

    ~Texture() { glDeleteTextures(1, &this->textureID); }

public:
    void
    setTextureUnit(GLint id, const std::string& uniformName, const Shader& shader) const {
        glActiveTexture(id);
        glBindTexture(GL_TEXTURE_2D, this->textureID);
        shader.use();
        shader.setInt(uniformName, id - GL_TEXTURE0);
    }
};

#endif //LEARNOPENGL_TEXTURE_HPP
