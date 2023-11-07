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
anya::Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
// 创建io设备
anya::Device device;
// path前缀
std::string prefix = "../src/4.advanced_opengl/4.8.2.geometry_shader_explode";


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void do_movement();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
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

    unsigned int skyboxVAO = 0, skyboxVBO = 0;

    // 创建vao并绑定
    glGenVertexArrays(1, &skyboxVAO);
    glBindVertexArray(skyboxVAO);

    // 创建vbo
    glGenBuffers(1, &skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    // 设置顶点属性指针
    // 每个顶点属性从一个VBO管理的内存中获得它的数据，
    // 而具体是从哪个VBO（程序中可以有多个VBO）获取
    // 则是通过在调用 glVertexAttribPointer 时绑定到 GL_ARRAY_BUFFER 的 VBO 决定的
    // 同时也会绑定到对应的EBO
    // 顶点位置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

//---------------------------------------------------------------------------------------------------------//

    anya::Model nanoModel("../art/model/nanosuit_reflection/nanosuit.obj");
    anya::Model lightModel("../art/model/cube/cube.obj");

//---------------------------------------------------------------------------------------------------------//

    std::vector<std::string> faces { prefix + "/right.jpg",
                                     prefix + "/left.jpg",
                                     prefix + "/top.jpg",
                                     prefix + "/bottom.jpg",
                                     prefix + "/front.jpg",
                                     prefix + "/back.jpg"};

    anya::CubeMap skybox(faces);

//---------------------------------------------------------------------------------------------------------//

    anya::Shader nanoShader(prefix + "/mainShader.vert", prefix + "/mainShader.frag", prefix + "/mainShader.geom");
    anya::Shader skyboxShader(prefix + "/skyboxShader.vert", prefix + "/skyboxShader.frag");
    anya::Shader lightShader(prefix + "/mainShader.vert", prefix + "/lightShader.frag");
    lightShader.isEnableLog = false;

    // 激活程序对象并设置uniform
    nanoShader.use();
    nanoShader.setVec3("viewPos", camera.position);
    nanoShader.setFloat("material.shininess", 32.0f);

    // 方向光
    anya::DirLight dirLight;
    dirLight.direction = glm::vec3( -0.2f, -1.0f, -0.3f);
    dirLight.ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    dirLight.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    dirLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    nanoShader.setDirLight(dirLight);

    // 聚光
    anya::SpotLight spotLight;
    spotLight.position = glm::vec3(1.0f);
    spotLight.direction = glm::vec3(1.0f);
    spotLight.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
    spotLight.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    spotLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    spotLight.cutOff = glm::cos(glm::radians(3.5f));
    spotLight.outerCutOff = glm::cos(glm::radians(5.5f));

    // 点光源
    anya::PointLight pointLight;
    pointLight.position = glm::vec3(1.0f);
    pointLight.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    pointLight.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    pointLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    pointLight.constant = 1.0f;
    pointLight.linear = 0.09f;
    pointLight.quadratic = 0.032f;

//---------------------------------------------------------------------------------------------------------//

    // 创建ubo并绑定
    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    // 开辟内存并绑定ubo到指定的索引
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // 绑定uniform块到特定的索引
    unsigned int uniformBlockIndexNano    = glGetUniformBlockIndex(nanoShader.shaderProgramID, "Matrices");
    unsigned int uniformBlockIndexLight   = glGetUniformBlockIndex(lightShader.shaderProgramID, "Matrices");
    glUniformBlockBinding(nanoShader.shaderProgramID, uniformBlockIndexNano, 0);
    glUniformBlockBinding(lightShader.shaderProgramID, uniformBlockIndexLight, 0);


//--------------------------------------------------------------------------------------------------------//

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
        glm::mat4 model = glm::mat4(1.0f);

        // 填充ubo
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // 环绕动作
        float radius = 2.0f;
        float camX = std::sin(glfwGetTime()) * radius;
        float camZ = std::cos(glfwGetTime()) * radius;

        // 聚光
        if (camera.isOpenSpotLight) {
            nanoShader.use();
            spotLight.position = camera.position;
            spotLight.direction = camera.front;
            nanoShader.setSpotLight(spotLight);
        }
        else {
            nanoShader.use();
            nanoShader.setSpotLight({});
        }

        // 点光源
        nanoShader.use();
        pointLight.position = glm::vec3(camX, 0.0f, camZ);
        // nanoShader.setPointLight(pointLight);

        // nano状态设置
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -4.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        nanoShader.setMatrix4fv("model", model);
        // nanoShader.setMatrix4fv("view", view);
        // nanoShader.setMatrix4fv("projection", projection);
        nanoShader.setVec3("cameraPos", camera.position);
        nanoShader.setCubeMapUnit(GL_TEXTURE3, "skybox", skybox);
        nanoShader.setFloat("time", glfwGetTime());
        nanoModel.draw(nanoShader);

        // light状态设置
        lightShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(camX, 0.0f, camZ));
        model = glm::scale(model, glm::vec3(0.1f));
        lightShader.setMatrix4fv("model", model);
        // lightShader.setMatrix4fv("view", view);
        // lightShader.setMatrix4fv("projection", projection);
        lightModel.draw(lightShader);

        // skybox状态设置
        skyboxShader.use();
        skyboxShader.setMatrix4fv("view", glm::mat4(glm::mat3(camera.getViewMatrix())));
        skyboxShader.setMatrix4fv("projection", projection);
        glBindVertexArray(skyboxVAO);
        skyboxShader.setCubeMapUnit(GL_TEXTURE0, "skybox", skybox);
        glDrawArrays(GL_TRIANGLES, 0, 36);


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
