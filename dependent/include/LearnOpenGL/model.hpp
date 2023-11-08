//
// Created by Anya on 2023/11/2.
//

#ifndef LEARNOPENGL_MODEL_HPP
#define LEARNOPENGL_MODEL_HPP

#include <vector>
#include "shader.hpp"
#include "texture.hpp"
#include "mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace anya {

    class Model {
    public:
        std::vector<Mesh> meshes;
        std::string directory;

    public:
        explicit Model(const std::string& path) {
            loadModel(path);
        }

    public:
        void
        draw(const Shader& shader) {
            for (const auto& mesh : this->meshes) {
                mesh.draw(shader);
            }
        }

    private:
        void
        loadModel(const std::string& path) {
            Assimp::Importer import;
            const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
                return;
            }

            directory = path.substr(0, path.find_last_of('/'));
            processNode(scene->mRootNode, scene);
        }

        void
        processNode(aiNode *node, const aiScene *scene) {
            // 处理节点的所有网格
            for (size_t i = 0; i < node->mNumMeshes; ++i) {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.push_back(processMesh(mesh, scene));
            }
            // 递归处理子节点
            for (size_t i = 0; i < node->mNumChildren; ++i) {
                processNode(node->mChildren[i], scene);
            }
        }

        Mesh
        processMesh(aiMesh *mesh, const aiScene *scene) {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indexes;
            std::vector<Texture> textures;

            for (size_t i = 0; i < mesh->mNumVertices; ++i) {
                Vertex vertex;
                // positions
                vertex.position.x = mesh->mVertices[i].x;
                vertex.position.y = mesh->mVertices[i].y;
                vertex.position.z = mesh->mVertices[i].z;

                // normals
                if (mesh->HasNormals()) {
                    vertex.normal.x = mesh->mNormals[i].x;
                    vertex.normal.y = mesh->mNormals[i].y;
                    vertex.normal.z = mesh->mNormals[i].z;
                }

                // 网格是否有纹理坐标？
                if (mesh->mTextureCoords[0]) {
                    vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
                    vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
                }
                else {
                    vertex.texCoords = glm::vec2(0.0f, 0.0f);
                }

                vertices.push_back(vertex);
            }

            for (size_t i = 0; i < mesh->mNumFaces; ++i) {
                aiFace face = mesh->mFaces[i];
                for (size_t j = 0; j < face.mNumIndices; ++j) {
                    indexes.push_back(face.mIndices[j]);
                }
            }

            if (mesh->mMaterialIndex >= 0) {
                aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

                std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
                textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

                std::vector<Texture> specularMaps = loadMaterialTextures(material,aiTextureType_SPECULAR, "specular");
                textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

                std::vector<Texture> ambientMaps = loadMaterialTextures(material,aiTextureType_AMBIENT, "ambient");
                textures.insert(textures.end(), ambientMaps.begin(), ambientMaps.end());
            }

            return Mesh{vertices, indexes, textures};
        }

        std::vector<Texture>
        loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName) {
            std::vector<Texture> textures;
            for (size_t i = 0; i < mat->GetTextureCount(type); ++i) {
                aiString str;
                mat->GetTexture(type, i, &str);
                Texture texture(this->directory + "/" + str.C_Str());
                texture.type = typeName;
                textures.push_back(texture);
            }
            return textures;
        }

    };

}


#endif //LEARNOPENGL_MODEL_HPP
