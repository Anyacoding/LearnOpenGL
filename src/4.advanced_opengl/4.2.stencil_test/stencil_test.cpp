#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <LearnOpenGL/shader.hpp>
#include <LearnOpenGL/camera.hpp>
#include <LearnOpenGL/device.hpp>
#include <LearnOpenGL/texture.hpp>
#include <array>

// stb的库像素数据都是从左到右，从上到下存储
// 我们要转为通用纹理坐标，左下角为(0,0) , 右上角为(width-1, height-1)
// include之前必须定义
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// 混合值
float k = 0.2;
// 当前帧与上一帧的时间差
float deltaTime = 0.0f;
// 上一帧的时间
float lastFrame = 0.0f;

// 创建摄像机
anya::Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
// 创建io设备
anya::Device device;
// path前缀
std::string prefix = "../src/4.advanced_opengl/4.2.stencil_test";

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void do_movement();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


float cubeVertices[] = {
        // positions          // texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

float planeVertices[] = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
        5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

        5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
        5.0f, -0.5f, -5.0f,  2.0f, 2.0f
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
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // 开启模板测试
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    // 隐藏鼠标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // 设置当窗口大小调整时的回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // 设置键盘输入的回调函数
    glfwSetKeyCallback(window, key_callback);
    // 设置鼠标输入的回调函数
    glfwSetCursorPosCallback(window, mouse_callback);
    // 设置滚轮输入的回调函数
    glfwSetScrollCallback(window, scroll_callback);

//---------------------------------------------------------------------------------------------------------//

    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    // 顶点属性
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // uv坐标属性
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);

//---------------------------------------------------------------------------------------------------------//

    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glBindVertexArray(planeVAO);

    glGenBuffers(1, &planeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    // 顶点属性
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // uv坐标属性
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);

//---------------------------------------------------------------------------------------------------------//

    anya::Shader shader(prefix + "/mainShader.vert", prefix + "/mainShader.frag");
    anya::Shader edgeShader(prefix + "/mainShader.vert", prefix + "/EdgeFragmentShader.glsl");

//--------------------------------------------------------------------------------------------------------//

    // 读入纹理src
    anya::Texture texture1(prefix + "/get.jpg");
    anya::Texture texture2(prefix + "/get.png");

//--------------------------------------------------------------------------------------------------------//

    // 渲染循环
    while(!glfwWindowShouldClose(window)) {
        // 渲染指令
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        // 清理每一帧的颜色缓存和深度缓存
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix();

        shader.use();
        shader.setMatrix4fv("view", view);
        shader.setMatrix4fv("projection", projection);

        // 记得保证我们在绘制地板的时候不会更新模板缓冲
        glStencilMask(0x00);
        // floor
        glBindVertexArray(planeVAO);
        shader.use();
        shader.setMatrix4fv("model", model);
        shader.setTextureUnit(GL_TEXTURE0, "texture1", texture2);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

//------------first pass-------------------------------------------------------------------------------------//

        // 所有的片段都应该更新模板缓冲
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        // 启用模板缓冲写入
        glStencilMask(0xFF);

        shader.use();
        // cubes
        glBindVertexArray(cubeVAO);
        shader.setTextureUnit(GL_TEXTURE0, "texture1", texture1);

        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        shader.setMatrix4fv("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        shader.setMatrix4fv("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

//-----------second pass--------------------------------------------------------------------------------------//

        // 通过的条件为模板值为 val != (1 & 0xFF)，也就是模板缓冲值为0时
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        // 禁用模板缓冲中所有位的写入
        glStencilMask(0x00);
        // 禁止深度测试，防止被地板挡住
        glDisable(GL_DEPTH_TEST);

        edgeShader.use();
        edgeShader.setMatrix4fv("view", view);
        edgeShader.setMatrix4fv("projection", projection);
        // 放大的cubes
        glBindVertexArray(cubeVAO);

        float scale = 1.05f;

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        model = glm::scale(model, glm::vec3(scale));
        edgeShader.setMatrix4fv("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale));
        edgeShader.setMatrix4fv("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 恢复模板写入和深度测试
        glStencilMask(0xFF);
        glEnable(GL_DEPTH_TEST);


        // 检查并调用事件，交换缓冲
        glfwSwapBuffers(window);
        // 处理输入事件
        glfwPollEvents();
        // 移动摄像机
        do_movement();
    }

    glfwTerminate();
    return 0;
}



void
framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void
do_movement() {
    if (device.keys[GLFW_KEY_W]) {
        camera.position += camera.speed * camera.front * deltaTime;
    }
    else if (device.keys[GLFW_KEY_S]) {
        camera.position -= camera.speed * camera.front * deltaTime;
    }
    else if (device.keys[GLFW_KEY_A]) {
        camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) * camera.speed * deltaTime;
    }
    else if (device.keys[GLFW_KEY_D]) {
        camera.position += glm::normalize(glm::cross(camera.front, camera.up)) * camera.speed * deltaTime;
    }
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

    if (action == GLFW_PRESS) {
        device.keys[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        device.keys[key] = false;
    }
}

void
mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (device.firstMouse) {
        device.lastX = xpos;
        device.lastY = ypos;
        device.firstMouse = false;
    }

    float xoffset = xpos - device.lastX;
    float yoffset = device.lastY - ypos;
    device.lastX = xpos;
    device.lastY = ypos;

    xoffset *= device.mouseSensitivity;
    yoffset *= device.mouseSensitivity;

    camera.yaw += xoffset;
    camera.pitch += yoffset;

    // 防万向锁
    camera.pitch = camera.pitch > 89.0f ? 89.0f : camera.pitch;
    camera.pitch = camera.pitch < -89.0f ? -89.0f : camera.pitch;

    glm::vec3 front;
    front.x = cos(glm::radians(camera.pitch)) * cos(glm::radians(camera.yaw));
    front.y = sin(glm::radians(camera.pitch));
    front.z = cos(glm::radians(camera.pitch)) * sin(glm::radians(camera.yaw));
    camera.front = glm::normalize(front);
}

void
scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.fov = camera.fov >= 1.0f && camera.fov <= 90.0f ? camera.fov - yoffset : camera.fov;
    camera.fov = camera.fov <= 1.0f ? 1.0f : camera.fov;
    camera.fov = camera.fov >= 90.0f ? 90.0f : camera.fov;
}