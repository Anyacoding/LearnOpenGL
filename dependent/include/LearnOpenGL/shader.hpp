//
// Created by Anya on 2023/10/22.
//

#ifndef LEARNOPENGL_SHADER_HPP
#define LEARNOPENGL_SHADER_HPP

#include <glad/glad.h>
#include <iostream>
#include <fstream>

class Shader {
public:
    unsigned int shaderProgramID = 0;

public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath) {
        // 读取 shader 源码
        std::string temp = loadShader(vertexPath);
        const char* vertexShaderSource = temp.c_str();

        // 创建 shader 对象
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

        // 读取 shader 源码
        temp = loadShader(fragmentPath);
        const char* fragmentShaderSource = temp.c_str();

        // 创建 shader 对象
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

        // 创建shader程序对象
        this->shaderProgramID = glCreateProgram();

        // 按顺序将之前编译的shader附加到程序对象上
        glAttachShader(this->shaderProgramID, vertexShader);
        glAttachShader(this->shaderProgramID, fragmentShader);
        glLinkProgram(this->shaderProgramID);

        // 检查链接错误
        glGetProgramiv(this->shaderProgramID, GL_LINK_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
    }

public:
    void
    use() const { glUseProgram(this->shaderProgramID); }

    void
    setBool(const std::string& name, bool value) const {
        int location = glGetUniformLocation(shaderProgramID, name.c_str());
        if(location == -1) {
            std::cout << "ERROR::SHADER::PROGRAM::UNIFORM_NOTFOUND\n" << std::endl;
            return;
        }
        glUniform1i(location, value);
    }

    void
    setInt(const std::string& name, int value) const {
        int location = glGetUniformLocation(shaderProgramID, name.c_str());
        if(location == -1) {
            std::cout << "ERROR::SHADER::PROGRAM::UNIFORM_NOTFOUND\n" << std::endl;
            return;
        }
        glUniform1i(location, value);
    }

    void
    setFloat(const std::string& name, float value) const {
        int location = glGetUniformLocation(shaderProgramID, name.c_str());
        if(location == -1) {
            std::cout << "ERROR::SHADER::PROGRAM::UNIFORM_NOTFOUND\n" << std::endl;
            return;
        }
        glUniform1f(location, value);
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

#endif //LEARNOPENGL_SHADER_HPP
