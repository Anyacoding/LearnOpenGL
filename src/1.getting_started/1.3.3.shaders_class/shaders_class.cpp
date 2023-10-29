#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <LearnOpenGL/shader.hpp>

void
framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void
processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

float vertices[] = {
        // 位置              // 颜色
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
};


unsigned int indices[] = {
        0, 1, 2 // 第一个三角形
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // 当目标是GL_ELEMENT_ARRAY_BUFFER的时候
    // VAO会储存glBindBuffer的函数调用。这也意味着它也会储存解绑调用，
    // 所以确保你没有在 解绑VAO之前 解绑索引数组缓冲，否则它就没有这个EBO配置了
    // 但是 VBO 没有这个限制
    glBindVertexArray(0);

//---------------------------------------------------------------------------------------------------------//

    Shader shader("../src/1.getting_started/1.3.3.shaders_class/VertexShader.glsl", "../src/1.getting_started/1.3.3.shaders_class/FragmentShader.glsl");

//--------------------------------------------------------------------------------------------------------//

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // 渲染循环
    while(!glfwWindowShouldClose(window)) {
        // 渲染指令
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 激活程序对象并
        shader.use();
        // 重新绑定VAO，来设置正确的索引关系
        glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)0);

        // 检查并调用事件，交换缓冲
        glfwSwapBuffers(window);
        glfwPollEvents();

        // 输入
        processInput(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader.shaderProgramID);
    glfwTerminate();

    return 0;
}

