//
// Created by Anya on 2023/10/31.
//

#ifndef LEARNOPENGL_MESH_HPP
#define LEARNOPENGL_MESH_HPP

#include <vector>
#include <cstddef>
#include <string>
#include "vertex.hpp"
#include "texture.hpp"
#include "shader.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace anya {

    class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indexes;
        std::vector<Texture> textures;

    private:
        unsigned int VAO = 0, VBO = 0, EBO = 0;

    public:
        Mesh(const std::vector<Vertex>&       vertices,
             const std::vector<unsigned int>& indexes,
             const std::vector<Texture>&      textures) :
             vertices(vertices),
             indexes(indexes),
             textures(textures) {
            // now that we have all the required data, set the vertex buffers and its attribute pointers.
            setupMesh();
        }

    public:
        void
        draw(const Shader& shader) const {
            std::size_t diffuseNr = 1;
            std::size_t specularNr = 1;

            for (std::size_t i = 0; i < this->textures.size(); ++i) {
                std::string uniformName;
                std::string type = this->textures[i].type;

                if (type == "diffuse") {
                    uniformName = "material." + type + std::to_string(diffuseNr++);
                }
                else if (type == "specular") {
                    uniformName = "material." + type + std::to_string(specularNr++);
                }

                shader.setTextureUnit(GL_TEXTURE0 + i, uniformName, this->textures[i]);
            }

            glBindVertexArray(this->VAO);
            glDrawElements(GL_TRIANGLES, this->indexes.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

    private:
        void
        setupMesh() {
            // 创建vao并绑定
            glGenVertexArrays(1, &this->VAO);
            glBindVertexArray(this->VAO);

            // 创建vbo
            glGenBuffers(1, &this->VBO);
            glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
            glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

            // 创建ebo
            glGenBuffers(1, &this->EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indexes.size() * sizeof(unsigned int), &this->indexes[0], GL_STATIC_DRAW);

            // 设置顶点属性指针
            // 每个顶点属性从一个VBO管理的内存中获得它的数据，
            // 而具体是从哪个VBO（程序中可以有多个VBO）获取
            // 则是通过在调用 glVertexAttribPointer 时绑定到 GL_ARRAY_BUFFER 的 VBO 决定的
            // 同时也会绑定到对应的EBO
            // 顶点位置
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            glEnableVertexAttribArray(0);
            // 顶点法线
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
            glEnableVertexAttribArray(1);
            // 顶点纹理坐标
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
            glEnableVertexAttribArray(2);

            // 当目标是GL_ELEMENT_ARRAY_BUFFER的时候
            // VAO会储存glBindBuffer的函数调用。这也意味着它也会储存解绑调用，
            // 所以确保你没有在 解绑VAO之前 解绑索引数组缓冲，否则它就没有这个EBO配置了
            // 但是 VBO 没有这个限制
            glBindVertexArray(0);
        }
    };
}

#endif //LEARNOPENGL_MESH_HPP
