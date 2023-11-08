#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <array>
#include <LearnOpenGL/shader.hpp>
#include <LearnOpenGL/camera.hpp>
#include <LearnOpenGL/device.hpp>
#include <LearnOpenGL/texture.hpp>
#include <LearnOpenGL/model.hpp>
#include <LearnOpenGL/light/pointLight.hpp>
#include <LearnOpenGL/light/spotLight.hpp>
#include <LearnOpenGL/cubeMap.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// 混合值
float k = 0.2;
// 当前帧与上一帧的时间差
float deltaTime = 0.0f;
// 上一帧的时间
float lastFrame = 0.0f;


// 光源的位置
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// 创建摄像机
anya::Camera camera(glm::vec3(0.0f, 0.0f, 55.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
// 创建io设备
anya::Device device;
// path前缀
std::string prefix = "../src/4.advanced_opengl/4.9.2.asteroids";


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void do_movement();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


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
    glDepthFunc(GL_LEQUAL);

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

    anya::Model planetModel("../art/model/planet/planet.obj");
    anya::Model rockModel("../art/model/rock/rock.obj");

//---------------------------------------------------------------------------------------------------------//

    unsigned int amount = 300000;
    glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
    float radius = 50.0;
    float offset = 2.5f;
    for (unsigned int i = 0; i < amount; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: Scale between 0.05 and 0.25f
        float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = static_cast<float>((rand() % 360));
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices[i] = model;
    }

    // 顶点缓冲对象
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    for(unsigned int i = 0; i < rockModel.meshes.size(); i++) {
        unsigned int VAO = rockModel.meshes[i].VAO;
        glBindVertexArray(VAO);
        // 顶点属性
        GLsizei vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));

        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));

        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

//---------------------------------------------------------------------------------------------------------//

    anya::Shader planetShader(prefix + "/planetShader.vert", prefix + "/planetShader.frag");
    anya::Shader asteroidShader(prefix + "/asteroidShader.vert", prefix + "/asteroidShader.frag");

//---------------------------------------------------------------------------------------------------------//

    // 渲染循环
    while(!glfwWindowShouldClose(window)) {
        // 渲染指令
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        // 清理每一帧的颜色缓存和深度缓存
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 计算deltaTime
        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glm::mat4 projection = camera.getProjectionMatrix();
        glm::mat4 view = camera.getViewMatrix();

        asteroidShader.use();
        asteroidShader.setMatrix4fv("projection", projection);
        asteroidShader.setMatrix4fv("view", view);
        planetShader.use();
        planetShader.setMatrix4fv("projection", projection);
        planetShader.setMatrix4fv("view", view);

        // draw planet
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        planetShader.setMatrix4fv("model", model);
        planetModel.draw(planetShader);

        // draw meteorites
        asteroidShader.use();
        for (unsigned int i = 0; i < rockModel.meshes.size(); i++) {
            glBindVertexArray(rockModel.meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(rockModel.meshes[i].indexes.size()), GL_UNSIGNED_INT, 0, amount);
            glBindVertexArray(0);
        }

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
    else if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        camera.isOpenSpotLight ^= true;
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
