#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <LearnOpenGL/shader.hpp>

// stb的库像素数据都是从左到右，从上到下存储
// 我们要转为通用纹理坐标，左下角为(0,0) , 右上角为(width-1, height-1)
// include之前必须定义
#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

float k = 0;

void
framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        k = std::min(1.0f, k + 0.1f);
    }
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        k = std::max(0.0f, k - 0.1f);
    }
    std::cout << k << std::endl;
}

float vertices[] = {
//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
};


unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
};

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口句柄并绑定到当前线程的主上下文
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 初始化glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 设置视口
    glViewport(0, 0, 800, 600);

    // 设置当窗口大小调整时的回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // 设置键盘输入的回调函数
    glfwSetKeyCallback(window, key_callback);

//---------------------------------------------------------------------------------------------------------//

    // 创建vao并绑定
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // 创建ebo并绑定
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // 将数据写到ebo上
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 创建vbo
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    // 绑定 vbo 到 GL_ARRAY_BUFFER 目标上，并把顶点数据写入到 GL_ARRAY_BUFFER 对应的缓存中
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    // 设置顶点属性指针
    // 每个顶点属性从一个VBO管理的内存中获得它的数据，
    // 而具体是从哪个VBO（程序中可以有多个VBO）获取
    // 则是通过在调用 glVertexAttribPointer 时绑定到 GL_ARRAY_BUFFER 的 VBO 决定的
    // 同时也会绑定到对应的EBO
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 纹理坐标属性
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // 当目标是GL_ELEMENT_ARRAY_BUFFER的时候
    // VAO会储存glBindBuffer的函数调用。这也意味着它也会储存解绑调用，
    // 所以确保你没有在 解绑VAO之前 解绑索引数组缓冲，否则它就没有这个EBO配置了
    // 但是 VBO 没有这个限制
    glBindVertexArray(0);

//---------------------------------------------------------------------------------------------------------//

    Shader shader("../src/1.getting_started/1.5.transformations/VertexShader.glsl", "../src/1.getting_started/1.5.transformations/FragmentShader.glsl");

//--------------------------------------------------------------------------------------------------------//

    // 读入纹理src
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("../src/1.getting_started/1.5.transformations/container.jpg", &width, &height, &nrChannels, 0);

    if (data == nullptr) {
        std::cerr << "ERROR IMAGE NOT FOUND" << std::endl;
    }

    // 生成纹理并绑定
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 载入纹理src并生成mipmap，注意此图的通道是RGB
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // 释放纹理src
    stbi_image_free(data);

    // 读入纹理src
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("../src/1.getting_started/1.5.transformations/awesomeface.png", &width, &height, &nrChannels, 0);

    if (data == nullptr) {
        std::cerr << "ERROR IMAGE NOT FOUND" << std::endl;
    }

    // 生成纹理并绑定
    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 载入纹理src并生成mipmap, 注意此图的通道是RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // 释放纹理src
    stbi_image_free(data);

//--------------------------------------------------------------------------------------------------------//

    // 激活纹理单元并绑定texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // 激活程序对象并设置纹理单元
    shader.use();
    shader.setInt("texture1", 0);
    shader.setInt("texture2", 1);

//--------------------------------------------------------------------------------------------------------//

    // 渲染循环
    while(!glfwWindowShouldClose(window)) {
        // 渲染指令
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        shader.setFloat("k", k);
        // 设置旋转矩阵
        // 实际的变换顺序应该与阅读顺序相反：
        // 尽管在代码中我们先位移再旋转，
        // 实际的变换却是先应用旋转再是位移的
        glm::mat4 trans;

        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        shader.setMatrix4fv("transform", trans);
        // 重新绑定VAO，来设置正确的索引关系
        glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

        // 检查并调用事件，交换缓冲
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader.shaderProgramID);
    glfwTerminate();

    return 0;
}
