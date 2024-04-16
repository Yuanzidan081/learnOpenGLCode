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
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
//============================ Function(Start) ============================
#pragma region

float mixFactor = 0.2;


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        mixFactor += 0.01;
        if (mixFactor >= 1.0)
            mixFactor = 1.0;
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        mixFactor -= 0.01;
        if (mixFactor <= 0.0)
            mixFactor = 0.0;
    }
}

const int screenWidth = 800;
const int screenHeight = 600;



#pragma endregion
//============================ Function(End) ============================

int main()
{

    //============================ 初始化(start) ============================
#pragma region
    GLFWwindow* window = GlInit(screenWidth, screenHeight, "LearnOpenGL");
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
            {"Texture", 2}
        });
    
    //创建纹理
    Texture tex1("assets/container.jpg", 0);
    Texture tex2("assets/awesomeface.png", 1);
    
#pragma endregion
    //============================ 顶点数据(end) ============================





    //============================ shader(start) ============================
#pragma region
    const char* vs = "assets/vertex_core.glsl";
    const char* fs = "assets/fragment_core.glsl";

    Shader shader(vs, fs);
    shader.useShader();
    //设置采样器
    shader.setInt("texture1", 0);
    shader.setInt("texture2", 1);
    //shader.setMat4("transform", trans);
#pragma endregion
    //============================ shader(end) ============================


    //============================ matrix(start) ============================
#pragma region
    glm::mat4 view(1.0);
    // 注意，我们将矩阵向我们要进行移动场景的反方向移动。
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection(1.0);
    projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
#pragma endregion
    //============================ matrix(end) ============================    

    
    //============================ 渲染循环(start) ============================
#pragma region
    while (!glfwWindowShouldClose(window))
    {
        //输入
        processInput(window);

        // 渲染指令-类似黑板的颜色
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //激活纹理
        tex1.Activate(0);
        tex2.Activate(1);

        //可以使用上一版的程序mixFactor定义在函数里面，这里将其设置在main函数外面是一个全局变量

        for (int i = 0; i < 10; ++i)
        {
            glm::mat4 model(1.0);
            float angle = 20.0f * i;
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        

        shader.setFloat("mixFactor", mixFactor);
        shader.useShader();
        vertex.BindVertex();
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        
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
