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





#pragma endregion
//============================ Function(End) ============================

int main()
{

    //============================ ��ʼ��(start) ============================
#pragma region
    GLFWwindow* window = GlInit(800, 600, "LearnOpenGL");
#pragma endregion
    //============================ ��ʼ��(end) ============================


    //============================ ��������(start) ============================
#pragma region
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);//����˳ʱ����ʾ����GL_BACK����ʱ����ʾ����GL_FRONT

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//�߿�ģʽ
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//Ĭ��ģʽ�������ɫ

    
    // vertices��indices����Model�ļ�����
    Vertex vertex(vertices, sizeof(vertices), indices, sizeof(indices));
    vertex.SetLayout(
        {
            {"Position", 3},
            {"Color", 3},
            {"Texture", 2}
        });
    
    //6.��������
    Texture tex1("assets/container.jpg", 0);
    Texture tex2("assets/awesomeface.png", 1);
    
#pragma endregion
    //============================ ��������(end) ============================





    //============================ shader(start) ============================
#pragma region
    const char* vs = "assets/vertex_core.glsl";
    const char* fs = "assets/fragment_core.glsl";

    Shader shader(vs, fs);
    shader.useShader();
    //���ò�����
    shader.setInt("texture1", 0);
    shader.setInt("texture2", 1);
    //shader.setMat4("transform", trans);
#pragma endregion
    //============================ shader(end) ============================

    
    //============================ ��Ⱦѭ��(start) ============================
#pragma region
    while (!glfwWindowShouldClose(window))
    {
        //����
        processInput(window);

        // ��Ⱦָ��-���ƺڰ����ɫ
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //ʹ��glUseProgram����������
        //��glUseProgram��������֮��ÿ����ɫ�����ú���Ⱦ���ö���ʹ������������Ҳ����֮ǰд����ɫ��)

        //��������
        tex1.Activate(0);
        tex2.Activate(1);

        //����ʹ����һ��ĳ���mixFactor�����ں������棬���ｫ��������main����������һ��ȫ�ֱ���
        glm::mat4 trans = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        shader.setMat4("transform", trans);
        shader.setFloat("mixFactor", mixFactor);
        shader.useShader();
        vertex.BindVertex();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        
        //����ǰ��֡
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#pragma endregion
    //============================ ��Ⱦѭ��(end) ============================
    vertex.DeleteVertex();
    shader.deleteShaderPrograme();
    glfwTerminate();
    return 0;
}
