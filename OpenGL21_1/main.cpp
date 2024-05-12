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
#include "ShaderSetting.h"
#include <vector>
#include "Importer.h"
#include "Base.h"

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
    

#pragma endregion
    //============================ 初始化(end) ============================


    //============================ 顶点数据(start) ============================
#pragma region
    // vertices和indices放在Model文件里了
    Mesh pointMesh(pointVertices, sizeof(pointVertices));
    pointMesh.SetLayout(
        {
            {"Position", 3},
            {"Color", 3},
        }
    );

    Mesh cubeMesh(vertices, sizeof(vertices));
    cubeMesh.SetLayout(
        {
            {"Position", 3},
            {"Normal", 3},
            {"Texture", 2}
        }
    );
#pragma endregion
    //============================ 顶点数据(end) ============================

//============================ 纹理(start) ============================
#pragma region

    Texture tex0("assets/container.jpg", 0, true);
    Texture tex1("assets/container2.png", 1, true);


#pragma endregion
    //============================ 纹理(end) ============================    

    //============================ shader(start) ============================
#pragma region


    Shader pointShader("assets/vertex_point.glsl", "assets/fragment_point.glsl");
    Shader fragCoordShader("assets/vertex_fragCoord.glsl", "assets/fragment_fragCoord.glsl");
    Shader frontFacingShader("assets/vertex_frontFacing.glsl", "assets/fragment_frontFacing.glsl");

    Shader redShader("assets/vertex_uniform.glsl", "assets/fragment_red.glsl");
    Shader greenShader("assets/vertex_uniform.glsl", "assets/fragment_green.glsl");
    Shader blueShader("assets/vertex_uniform.glsl", "assets/fragment_blue.glsl");
    Shader yellowShader("assets/vertex_uniform.glsl", "assets/fragment_yellow.glsl");

#pragma endregion
    //============================ shader(end) ============================


//============================ uniform块的设置(start) ============================
#pragma region
    unsigned int uniformBlockIndexRed = glGetUniformBlockIndex(redShader.getShaderPrograme(), "Matrices");
    unsigned int uniformBlockIndexGreen = glGetUniformBlockIndex(greenShader.getShaderPrograme(), "Matrices");
    unsigned int uniformBlockIndexBlue = glGetUniformBlockIndex(blueShader.getShaderPrograme(), "Matrices");
    unsigned int uniformBlockIndexYellow = glGetUniformBlockIndex(yellowShader.getShaderPrograme(), "Matrices");

    glUniformBlockBinding(redShader.getShaderPrograme(), uniformBlockIndexRed, 0);
    glUniformBlockBinding(greenShader.getShaderPrograme(), uniformBlockIndexGreen, 0);
    glUniformBlockBinding(blueShader.getShaderPrograme(), uniformBlockIndexBlue, 0);
    glUniformBlockBinding(yellowShader.getShaderPrograme(), uniformBlockIndexYellow, 0);

    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

    glm::mat4 projection_uniform = glm::perspective(glm::radians(camera.m_Fov), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection_uniform));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
#pragma endregion
//============================ uniform块的设置(end) ============================


    
    //============================ 渲染循环(start) ============================
#pragma region
    while (!glfwWindowShouldClose(window))
    {
        //输入
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glm::mat4 model(1.0);
        glm::mat4 view = camera.GetCameraView();
        glm::mat4 projection = glm::perspective(glm::radians(camera.m_Fov), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);

        

        glDepthMask(GL_TRUE);
         
        // 第一阶段
        glEnable(GL_DEPTH_TEST);// 绘制cube和灯开启深度测试
        // 渲染指令-类似黑板的颜色
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 现在不使用模板缓冲
               
        //绘制点
        glEnable(GL_PROGRAM_POINT_SIZE);
        pointShader.useShader();
        pointShader.setMat4("view", view);
        pointShader.setMat4("projection", projection);
        pointMesh.BindVertex();
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, -1.1f));
        pointShader.setMat4("model", model);
        glDrawArrays(GL_POINTS, 0, 6);
        pointMesh.UnBindVertex();
        glDisable(GL_PROGRAM_POINT_SIZE);
       
        fragCoordShader.useShader();
        fragCoordShader.setMat4("view", view);
        fragCoordShader.setMat4("projection", projection);
        cubeMesh.BindVertex();
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, -3.0f));
        fragCoordShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        cubeMesh.UnBindVertex();

        frontFacingShader.useShader();
        frontFacingShader.setMat4("view", view);
        frontFacingShader.setMat4("projection", projection);
        cubeMesh.BindVertex();
        model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, -3.0f));
        frontFacingShader.setMat4("model", model);
        tex0.Activate(0);
        frontFacingShader.setInt("frontTexture", 0);
        tex1.Activate(1);
        frontFacingShader.setInt("backTexture", 1);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        cubeMesh.UnBindVertex();

        glm::mat4 view_uniform = camera.GetCameraView();
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view_uniform));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // uniform 块红色立方体
        cubeMesh.BindVertex();
        redShader.useShader();
        model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, -3.0f));
        redShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        // uniform 块绿色立方体
        greenShader.useShader();
        model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, -1.0f));
        greenShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // uniform 块蓝色立方体
        blueShader.useShader();
        model = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, -3.0f));
        blueShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // uniform 块黄色立方体
        yellowShader.useShader();
        model = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, -1.0f));
        yellowShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        cubeMesh.UnBindVertex();


        //交换前后帧
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#pragma endregion
    //============================ 渲染循环(end) ============================
    pointMesh.DeleteVertex();

    pointShader.deleteShaderPrograme();


    glfwTerminate();
    return 0;
}
