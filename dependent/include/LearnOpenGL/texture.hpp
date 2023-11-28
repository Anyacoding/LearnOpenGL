//
// Created by Anya on 2023/10/28.
//

#ifndef LEARNOPENGL_TEXTURE_HPP
#define LEARNOPENGL_TEXTURE_HPP

#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#include <glm/glm.hpp>

namespace anya {

    class Shader;

    class Texture {
    public:
         friend Shader;

    public:
        std::string type = "diffuse";

    public:
        int width = 800, height = 600, nrChannels = 3;
        unsigned int textureID = -1;
        GLint format = GL_RGB;

    public:
        explicit Texture(const std::string& path, bool flip = true, bool hdr = false) {
            if (hdr) {
                load_hdr(path, flip);
            }
            else {
                load_ldr(path, flip);
            }
        }

        // ~Texture() { glDeleteTextures(1, &this->textureID); }

    public:
        void load_ldr(const std::string& path, bool flip = true) {
            stbi_set_flip_vertically_on_load(flip);
            unsigned char *data = stbi_load(path.c_str(), &this->width, &this->height, &this->nrChannels, 0);

            if (data == nullptr) {
                std::cerr << "ERROR IMAGE "  << path << " NOT FOUND" << std::endl;
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

            // 载入纹理src并生成mipmap
            glTexImage2D(GL_TEXTURE_2D, 0, this->format, this->width, this->height, 0, this->format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            // 释放纹理src
            stbi_image_free(data);
        }

        void load_hdr(const std::string& path, bool flip = true) {
            stbi_set_flip_vertically_on_load(flip);
            float* data = stbi_loadf(path.c_str(), &this->width, &this->height, &this->nrChannels, 0);

            if (data == nullptr) {
                std::cerr << "ERROR IMAGE "  << path << " NOT FOUND" << std::endl;
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
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // 载入纹理src并生成mipmap
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, this->width, this->height, 0, this->format, GL_FLOAT, data);

            // 释放纹理src
            stbi_image_free(data);
        }

        void
        setWrapMode(GLint mode) const {
            glBindTexture(GL_TEXTURE_2D, this->textureID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
        }
    };

}

#endif //LEARNOPENGL_TEXTURE_HPP
