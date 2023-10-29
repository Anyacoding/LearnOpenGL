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

// 光源的位置
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// 创建摄像机
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
// 创建io设备
Device device;
// path前缀
std::string prefix = "../src/2.lighting/2.5.3.light_casters_spot";


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

float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
};

glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
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

    // 创建vao并绑定
    unsigned int cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

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
    // 法线属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // lighting_map坐标属性
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 当目标是GL_ELEMENT_ARRAY_BUFFER的时候
    // VAO会储存glBindBuffer的函数调用。这也意味着它也会储存解绑调用，
    // 所以确保你没有在 解绑VAO之前 解绑索引数组缓冲，否则它就没有这个EBO配置了
    // 但是 VBO 没有这个限制
    glBindVertexArray(0);

//---------------------------------------------------------------------------------------------------------//

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // 只需要绑定VBO不用再次设置VBO的数据，因为箱子的VBO数据中已经包含了正确的立方体顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 设置灯立方体的顶点属性（对我们的灯来说仅仅只有位置数据）
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

//---------------------------------------------------------------------------------------------------------//

    Shader cubeShader(prefix + "/VertexShader.glsl", prefix + "/FragmentShader.glsl");
    Shader lightShader(prefix + "/VertexShader.glsl", prefix + "/LightFragmentShader.glsl");

//--------------------------------------------------------------------------------------------------------//

    // 读入纹理src
    Texture texture1(prefix + "/container2.png");
    Texture texture2(prefix + "/container2_specular.png");
    Texture texture3(prefix + "/animation.jpg");

    // 设置纹理单元
    texture1.setTextureUnit(GL_TEXTURE0, "material.diffuse", cubeShader);
    texture2.setTextureUnit(GL_TEXTURE1, "material.specular", cubeShader);
    texture3.setTextureUnit(GL_TEXTURE2, "animation", cubeShader);

//---------------------------------------------------------------------------------------------------------//

    // 激活程序对象并设置uniform
    cubeShader.use();
    cubeShader.setVec3("viewPos", camera.position);
    cubeShader.setFloat("material.shininess", 32.0f);

    // 方向光
    cubeShader.setVec3("dirLight.direction", glm::vec3( -0.2f, -1.0f, -0.3f));
    cubeShader.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    cubeShader.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
    cubeShader.setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

    // 点光源
    cubeShader.setVec3("pointLights[0].position", pointLightPositions[0]);
    cubeShader.setVec3("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    cubeShader.setVec3("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    cubeShader.setVec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    cubeShader.setFloat("pointLights[0].constant", 1.0f);
    cubeShader.setFloat("pointLights[0].linear", 0.09f);
    cubeShader.setFloat("pointLights[0].quadratic", 0.032f);

    cubeShader.setVec3("pointLights[1].position", pointLightPositions[1]);
    cubeShader.setVec3("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    cubeShader.setVec3("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    cubeShader.setVec3("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    cubeShader.setFloat("pointLights[1].constant", 1.0f);
    cubeShader.setFloat("pointLights[1].linear", 0.09f);
    cubeShader.setFloat("pointLights[1].quadratic", 0.032f);

    cubeShader.setVec3("pointLights[2].position", pointLightPositions[2]);
    cubeShader.setVec3("pointLights[2].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    cubeShader.setVec3("pointLights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    cubeShader.setVec3("pointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    cubeShader.setFloat("pointLights[2].constant", 1.0f);
    cubeShader.setFloat("pointLights[2].linear", 0.09f);
    cubeShader.setFloat("pointLights[2].quadratic", 0.032f);

    cubeShader.setVec3("pointLights[3].position", pointLightPositions[3]);
    cubeShader.setVec3("pointLights[3].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    cubeShader.setVec3("pointLights[3].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    cubeShader.setVec3("pointLights[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    cubeShader.setFloat("pointLights[3].constant", 1.0f);
    cubeShader.setFloat("pointLights[3].linear", 0.09f);
    cubeShader.setFloat("pointLights[3].quadratic", 0.032f);

    // 聚光
    cubeShader.setVec3("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
    cubeShader.setVec3("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
    cubeShader.setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

//--------------------------------------------------------------------------------------------------------//

    // 渲染循环
    while(!glfwWindowShouldClose(window)) {
        // 渲染指令
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        // 清理每一帧的颜色缓存和深度缓存
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        cubeShader.use();
        cubeShader.setVec3("spotLight.position", camera.position);
        cubeShader.setVec3("spotLight.direction", camera.front);
        cubeShader.setFloat("spotLight.cutOff",   glm::cos(glm::radians(12.5f)));
        cubeShader.setFloat("spotLight.outerCutOff",   glm::cos(glm::radians(15.0f)));

        glm::mat4 model;

        // 设置cubeShader的状态
        for (int i = 0; i < 10; i++) {
            cubeShader.use();
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            cubeShader.setMatrix4fv("model", model);
            cubeShader.setMatrix4fv("view", camera.getViewMatrix());
            cubeShader.setMatrix4fv("projection", camera.getProjectionMatrix());
            // 重新绑定cubeVAO，来设置正确的索引关系
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // 设置lightShader的状态
        for (int i = 0; i < 4; ++i) {
            lightShader.use();
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lightShader.setMatrix4fv("model", model);
            lightShader.setMatrix4fv("view", camera.getViewMatrix());
            lightShader.setMatrix4fv("projection", camera.getProjectionMatrix());
            // 重新绑定lightVAO，来设置正确的索引关系
            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
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