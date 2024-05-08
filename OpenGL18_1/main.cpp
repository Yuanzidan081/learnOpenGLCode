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
#include "Base.h"
#include "ShaderSetting.h"
#include <vector>
#include <map>
//============================ Function(Start) ============================
#pragma region

const int screenWidth = 800;
const int screenHeight = 600;

int specFactor = 32;
bool firstMouse = true;
float lastX = (float)screenWidth / 2, lastY = (float)screenHeight / 2;
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间


// 10个物体的相机设置
Camera camera(glm::vec3(5.74329f, 2.40024f, 3.36758f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    587.843f,
    -15.5f);
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        specFactor += 1;
        if (specFactor >= 256)
            specFactor = 256;
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        specFactor -= 1;
        if (specFactor <= 1)
            specFactor = 1;
    }
    //std::cout << specFactor << std::endl;
    CameraDir dir;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        dir = Forward;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        dir = Backward;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        dir = Left;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        dir = Right;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        dir = Up;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        dir = Down;
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
            /*std::cout << camera.m_Yaw << " " << camera.m_Pitch << " "
                << camera.m_Pos.x << " "  << camera.m_Pos.y  << " " << camera.m_Pos.z << std::endl;*/
        });
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
        {
            camera.MouseScroolEvent(yoffset);
        });
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //背向面剔除
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);// 只删除背向面
    glFrontFace(GL_CW); // 定义顺时针为前向面
#pragma endregion
    //============================ 初始化(end) ============================


    //============================ 顶点数据(start) ============================
#pragma region
    // vertices和indices放在Model文件里了
    Mesh mesh(vertices, sizeof(vertices));
    mesh.SetLayout(
        {
            {"Position", 3},
            {"Normal", 3},
            {"Texture", 2}
        });
    //草的位置
    Mesh grassMesh(transparentVertices, sizeof(transparentVertices));
    grassMesh.SetLayout(
        {
            {"Position", 3},
            {"Texture", 2}
        });
    vector<glm::vec3> windows;
    windows.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
    windows.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
    windows.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
    windows.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
    windows.push_back(glm::vec3(0.5f, 0.0f, -0.6f));
#pragma endregion
    //============================ 顶点数据(end) ============================


    //============================ 纹理(start) ============================
#pragma region
    Texture tex0("assets/container2.png", 0);
    Texture tex1("assets/container2_specular.png", 1);
    Texture tex2("assets/texture1.jpg", 2);
    Texture tex3("assets/blending_transparent_window.png", 3, false);
#pragma endregion
    //============================ 纹理(end) ============================    


    //============================ shader(start) ============================
#pragma region

    // model空间下的cube的phong shader，本代码的设置只是针对这个shader
    Shader shader("assets/vertex_multiLight.glsl", "assets/fragment_multiLight.glsl");
    Shader shaderBlend("assets/vertex_blend.glsl", "assets/fragment_blend.glsl");
    //非线性的深度测试的shader
    /*Shader shader("assets/vertex_depth_test.glsl", "assets/fragment_depth_test.glsl");*/
    // 线性化的深度测试的shader
    //Shader shader("assets/vertex_depth_test.glsl", "assets/fragment_depth_linear_test.glsl");
    Shader shaderLight("assets/vertex_light.glsl", "assets/fragment_light.glsl");
#pragma endregion
    //============================ shader(end) ============================

    
    //============================ 渲染循环(start) ============================
#pragma region
    while (!glfwWindowShouldClose(window))
    {
        //输入
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        

        std::map<float, glm::vec3> sorted;
        for (unsigned int i = 0; i < windows.size(); ++i)
        {
            float dist = glm::length(camera.m_Pos - windows[i]);
            sorted[dist] = windows[i];
        }
        // 渲染指令-类似黑板的颜色
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glm::mat4 model(1.0);
        glm::mat4 view = camera.GetCameraView();
        glm::mat4 projection = glm::perspective(glm::radians(camera.m_Fov), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        

        //绘制cube
        setMultiLightShader(shader);

        //shader.useShader();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        mesh.BindVertex();
     

        tex0.Activate(0);// 激活纹理坐标
        tex1.Activate(1);
        tex2.Activate(2);

        // 地板
        shader.setInt("material.diffuse", 2);
        shader.setInt("material.specular", 2);
        model = scale(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 10.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        

        // 立方体1
        shader.setInt("material.diffuse", 0);
        shader.setInt("material.specular", 1);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, -1.1f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 立方体2
        shader.setInt("material.diffuse", 0);
        shader.setInt("material.specular", 1);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 0.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        mesh.UnBindVertex();

        // 绘制草/窗户

        shaderBlend.useShader();
        tex3.Activate(3);
        grassMesh.BindVertex();
        shaderBlend.setMat4("view", view);
        shaderBlend.setMat4("projection", projection);
        shaderBlend.setInt("texture1", 3);

        // 绘制无排序的透明物体
        /*for (unsigned int i = 0; i < windows.size(); i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, windows[i] + glm::vec3(0.0f, 1.0f, 0.0f));
            shaderBlend.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }*/

        // 绘制排序的透明物体
        for (auto it = sorted.rbegin(); it != sorted.rend(); ++it)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->second + glm::vec3(0.0f, 1.0f, 0.0f));
            shaderBlend.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        grassMesh.UnBindVertex();
        
        //绘制light
        shaderLight.useShader();
        model = glm::mat4(1.0);
        mesh.BindVertex();
        
        shaderLight.setMat4("view", view);
        shaderLight.setMat4("projection", projection);
        for (int i = 0; i < 4; ++i)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            shaderLight.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
               
        
        //交换前后帧
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#pragma endregion
    //============================ 渲染循环(end) ============================
    mesh.DeleteVertex();
    shader.deleteShaderPrograme();
    glfwTerminate();
    return 0;
}
