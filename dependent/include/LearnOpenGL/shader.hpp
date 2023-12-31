//
// Created by Anya on 2023/10/22.
//

#ifndef LEARNOPENGL_SHADER_HPP
#define LEARNOPENGL_SHADER_HPP

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <LearnOpenGL/light/pointLight.hpp>
#include <LearnOpenGL/light/spotLight.hpp>
#include <LearnOpenGL/light/directionLight.hpp>
#include <LearnOpenGL/texture.hpp>
#include <LearnOpenGL/cubeMap.hpp>

namespace anya {

    class Shader {
    public:
        unsigned int shaderProgramID = 0;
        bool isEnableLog = true;

    public:
        Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "") {
            // 读取 vertex shader 源码
            std::string temp = loadShader(vertexPath);
            const char* vertexShaderSource = temp.c_str();

            // 创建 vertex shader 对象
            unsigned int vertexShader;
            vertexShader = glCreateShader(GL_VERTEX_SHADER);

            // vertex shader源码附加到vertex shader对象
            glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
            glCompileShader(vertexShader);

            // 检查vertex shader是否编译成功
            int  success;
            char infoLog[512];
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
                std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            }

//--------------------------------------------------------------------------------------------------//

            // 读取 fragment shader 源码
            temp = loadShader(fragmentPath);
            const char* fragmentShaderSource = temp.c_str();

            // 创建 fragment shader 对象
            unsigned int fragmentShader;
            fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

            // fragment shader源码附加到fragment shader对象
            glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
            glCompileShader(fragmentShader);

            // 检查fragment shader是否编译成功
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
                std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
            }

//---------------------------------------------------------------------------------------------------//

            const char* geometryShaderSource = nullptr;
            unsigned int geometryShader;

            if (!geometryPath.empty()) {
                // 读取 geometry shader 源码
                temp = loadShader(geometryPath);
                geometryShaderSource = temp.c_str();

                // 创建 geometry shader 对象
                geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

                // geometry shader源码附加到geometry shader对象
                glShaderSource(geometryShader, 1, &geometryShaderSource, nullptr);
                glCompileShader(geometryShader);

                // 检查geometry shader是否编译成功
                glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
                if (!success) {
                    glGetShaderInfoLog(geometryShader, 512, nullptr, infoLog);
                    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
                }
            }

//---------------------------------------------------------------------------------------------------//

            // 创建shader程序对象
            this->shaderProgramID = glCreateProgram();

            // 按顺序将之前编译的shader附加到程序对象上
            glAttachShader(this->shaderProgramID, vertexShader);
            glAttachShader(this->shaderProgramID, fragmentShader);
            if (!geometryPath.empty()) { glAttachShader(this->shaderProgramID, geometryShader); }
            glLinkProgram(this->shaderProgramID);

            // 检查链接错误
            glGetProgramiv(this->shaderProgramID, GL_LINK_STATUS, &success);
            if(!success) {
                glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
                std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            }
        }

        // ~Shader() { glDeleteShader(this->shaderProgramID); }

    public:
        void
        use() const { glUseProgram(this->shaderProgramID); }

        void
        setBool(const std::string& name, bool value) const {
            int location = glGetUniformLocation(shaderProgramID, name.c_str());
            if(location == -1 && isEnableLog) {
                std::cout << "ERROR::SHADER::PROGRAM::UNIFORM_NOTFOUND " << name << std::endl;
                return;
            }
            glUniform1i(location, value);
        }

        void
        setInt(const std::string& name, int value) const {
            int location = glGetUniformLocation(shaderProgramID, name.c_str());
            if(location == -1 && isEnableLog) {
                std::cout << "ERROR::SHADER::PROGRAM::UNIFORM_NOTFOUND " << name << std::endl;
                return;
            }
            glUniform1i(location, value);
        }

        void
        setFloat(const std::string& name, float value) const {
            int location = glGetUniformLocation(shaderProgramID, name.c_str());
            if(location == -1 && isEnableLog) {
                std::cout << "ERROR::SHADER::PROGRAM::UNIFORM_NOTFOUND " << name << std::endl;
                return;
            }
            glUniform1f(location, value);
        }

        void
        setMatrix4fv(const std::string& name, glm::mat4 value) const {
            int location = glGetUniformLocation(shaderProgramID, name.c_str());
            if(location == -1 && isEnableLog) {
                std::cout << "ERROR::SHADER::PROGRAM::UNIFORM_NOTFOUND " << name << std::endl;
                return;
            }
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
        }

        void
        setVec3(const std::string& name, glm::vec3 value) const {
            int location = glGetUniformLocation(shaderProgramID, name.c_str());
            if(location == -1 && isEnableLog) {
                std::cout << "ERROR::SHADER::PROGRAM::UNIFORM_NOTFOUND " << name << std::endl;
                return;
            }
            glUniform3fv(location, 1, glm::value_ptr(value));
        }

        void
        setDirLight(const DirLight& dirLight) const {
            setVec3("dirLight.direction", dirLight.direction);
            setVec3("dirLight.ambient", dirLight.ambient);
            setVec3("dirLight.diffuse", dirLight.diffuse);
            setVec3("dirLight.specular", dirLight.specular);
        }

        void
        setPointLight(const PointLight& pointLight) const {
            setVec3("pointLight.position", pointLight.position);
            setVec3("pointLight.ambient", pointLight.ambient);
            setVec3("pointLight.diffuse", pointLight.diffuse);
            setVec3("pointLight.specular", pointLight.specular);
            setFloat("pointLight.constant", pointLight.constant);
            setFloat("pointLight.linear", pointLight.linear);
            setFloat("pointLight.quadratic", pointLight.quadratic);
        }

        void
        setSpotLight(const SpotLight& spotLight) const {
            setVec3("spotLight.position", spotLight.position);
            setVec3("spotLight.direction", spotLight.direction);
            setVec3("spotLight.ambient", spotLight.ambient);
            setVec3("spotLight.diffuse", spotLight.diffuse);
            setVec3("spotLight.specular", spotLight.specular);
            setFloat("spotLight.cutOff", spotLight.cutOff);
            setFloat("spotLight.outerCutOff", spotLight.outerCutOff);
        }

        void
        setTextureUnit(GLint id, const std::string& uniformName, const Texture& texture) const {
            glActiveTexture(id);
            glBindTexture(GL_TEXTURE_2D, texture.textureID);
            use();
            setInt(uniformName, id - GL_TEXTURE0);
        }

        void
        setCubeMapUnit(GLint id, const std::string& uniformName, const CubeMap& cubeMap) const {
            glActiveTexture(id);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.textureID);
            use();
            setInt(uniformName, id - GL_TEXTURE0);
        }

    private:
        static std::string
        loadShader(const std::string& path) {
            std::ifstream ifs(path, std::ios::binary | std::ios::ate);
            if (!ifs.is_open()) {
                std::cout << "open " << path << " fail!" << std::endl;
            }
            else {
                auto size = ifs.tellg();
                std::string str(size, '\0'); // 构造 string 为流大小
                ifs.seekg(0);
                if(ifs.read(&str[0], size)) {
                    ifs.close();
                    return str;
                }
            }
            ifs.close();
            return {};
        }
    };

}

#endif //LEARNOPENGL_SHADER_HPP
