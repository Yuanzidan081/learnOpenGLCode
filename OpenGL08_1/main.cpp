#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Vertex.h"
#include "Texture.h"
#include "Model.h"
#include "GlInit.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
//============================ Function(Start) ============================
#pragma region

const int screenWidth = 800;
const int screenHeight = 600;

float mixFactor = 0.2;
bool firstMouse = true;
float lastX = (float)screenWidth / 2, lastY = (float)screenHeight / 2;
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

// 全局只有一个相机
Camera camera(glm::vec3(3.7342, 2.0652f, -1.6181f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    144.3973f,
    -26.8f);
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        mixFactor += 0.01;
        if (mixFactor >= 1.0)
            mixFactor = 1.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        mixFactor -= 0.01;
        if (mixFactor <= 0.0)
            mixFactor = 0.0;
    }
    
    CameraDir dir;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        dir = Forward;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        dir = Backward;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        dir = Left;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        dir = Right;
    camera.KeyBoardPressEvent(dir, deltaTime);
}





#pragma endregion
//============================ Function(End) ============================

int main()
{

    //============================ 初始化(start) ============================
#pragma region
    GLFWwindow* window = GlInit(screenWidth, screenHeight, "LearnOpenGL");
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {glViewport(0, 0, width, height); });//自适应窗口大小
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
        {
            if (firstMouse)
            {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }
            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos; // glfw的x轴是向右的，y轴是向下的，原点在左上角
            lastX = xpos;
            lastY = ypos;

            camera.MouseMovementEvent(xoffset, yoffset);
        });
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
        {
            camera.MouseScroolEvent(yoffset);
        });
    glEnable(GL_DEPTH_TEST);
#pragma endregion
    //============================ 初始化(end) ============================


    //============================ 顶点数据(start) ============================
#pragma region
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);//顶点顺时针显示反面GL_BACK，逆时针显示正面GL_FRONT

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//线框模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//默认模式：填充颜色

    
    // vertices和indices放在Model文件里了
    Vertex vertex(vertices, sizeof(vertices));
    vertex.SetLayout(
        {
            {"Position", 3},
        });
    
    
#pragma endregion
    //============================ 顶点数据(end) ============================





    //============================ shader(start) ============================
#pragma region

    Shader shader("assets/vertex_core.glsl", "assets/fragment_core.glsl");
    Shader shaderLight("assets/vertex_light.glsl", "assets/fragment_light.glsl");

#pragma endregion
    //============================ shader(end) ============================


    //============================ 光源(start) ============================
#pragma region
    
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
#pragma endregion
    //============================ 光源(end) ============================    

    
    //============================ 渲染循环(start) ============================
#pragma region
    while (!glfwWindowShouldClose(window))
    {
        //输入
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // 渲染指令-类似黑板的颜色
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //绘制矩形
        shader.useShader();
        shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        glm::mat4 model(1.0);
        glm::mat4 view = camera.GetCameraView();
        glm::mat4 projection = glm::perspective(glm::radians(camera.m_Fov), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        vertex.BindVertex();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        //绘制灯
        shaderLight.useShader();
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        shaderLight.setMat4("model", model);
        shaderLight.setMat4("view", view);
        shaderLight.setMat4("projection", projection);
        vertex.BindVertex();
        glDrawArrays(GL_TRIANGLES, 0, 36);        
        
        //交换前后帧
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#pragma endregion
    //============================ 渲染循环(end) ============================
    vertex.DeleteVertex();
    shader.deleteShaderPrograme();
    glfwTerminate();
    return 0;
}
