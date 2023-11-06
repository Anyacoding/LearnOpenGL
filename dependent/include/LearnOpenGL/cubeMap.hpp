//
// Created by Anya on 2023/11/6.
//

#ifndef LEARNOPENGL_CUBEMAP_HPP
#define LEARNOPENGL_CUBEMAP_HPP

#include <vector>

namespace anya {

    class Shader;

    class CubeMap {
    public:
        friend Shader;

    private:
        int width = 800;
        int height = 600;
        int nrChannels = 3;
        GLint format = GL_RGB;
        GLint wrapMode = GL_REPEAT;

    public:
        unsigned int textureID = -1;

    public:
        explicit CubeMap(const std::vector<std::string>& faces, bool flip = false) {
            glGenTextures(1, &this->textureID);
            glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
            stbi_set_flip_vertically_on_load(flip);

            for (size_t i = 0; i < faces.size(); ++i) {
                unsigned char *data = stbi_load(faces[i].c_str(), &this->width, &this->height, &this->nrChannels, 0);

                if (data == nullptr) {
                    std::cerr << "ERROR IMAGE "  << faces[i] << " NOT FOUND" << std::endl;
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

                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, this->format, this->width, this->height,
                             0, this->format, GL_UNSIGNED_BYTE, data);

                stbi_image_free(data);
            }
        }
    };

}

#endif //LEARNOPENGL_CUBEMAP_HPP
