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

int specFactor = 32;
bool firstMouse = true;
float lastX = (float)screenWidth / 2, lastY = (float)screenHeight / 2;
float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��

// ȫ��ֻ��һ�����
Camera camera(glm::vec3(-3.7935f, 1.68357f, -2.39135f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    389.845f,
    -11.85f);
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
    std::cout << specFactor << std::endl;
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

    //============================ ��ʼ��(start) ============================
#pragma region
    GLFWwindow* window = GlInit(screenWidth, screenHeight, "LearnOpenGL");
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {glViewport(0, 0, width, height); });//����Ӧ���ڴ�С
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
        {
            if (firstMouse)
            {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }
            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos; // glfw��x�������ҵģ�y�������µģ�ԭ�������Ͻ�
            lastX = xpos;
            lastY = ypos;

            camera.MouseMovementEvent(xoffset, yoffset);
            std::cout << camera.m_Yaw << " " << camera.m_Pitch << " " 
                << camera.m_Pos.x << " "  << camera.m_Pos.y  << " " << camera.m_Pos.z << std::endl;
        });
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
        {
            camera.MouseScroolEvent(yoffset);
        });
    glEnable(GL_DEPTH_TEST);

    
#pragma endregion
    //============================ ��ʼ��(end) ============================


    //============================ ��������(start) ============================
#pragma region
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);//����˳ʱ����ʾ����GL_BACK����ʱ����ʾ����GL_FRONT

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//�߿�ģʽ
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//Ĭ��ģʽ�������ɫ

    
    // vertices��indices����Model�ļ�����
    Vertex vertex(vertices, sizeof(vertices));
    vertex.SetLayout(
        {
            {"Position", 3},
            {"Normal", 3}
        });
    
    
#pragma endregion
    //============================ ��������(end) ============================





    //============================ shader(start) ============================
#pragma region

    // model�ռ��µ�cube��phong shader
    //Shader shader("assets/vertex_core.glsl", "assets/fragment_core.glsl");
    // view�ռ��µ�cube��phong shader�������������ֻ��������shader
    Shader shader("assets/vertex_core_view.glsl", "assets/fragment_core_view.glsl");
    
    
    Shader shaderLight("assets/vertex_light.glsl", "assets/fragment_light.glsl");

#pragma endregion
    //============================ shader(end) ============================


    //============================ ��Դ(start) ============================
#pragma region
    /*glm::vec3 lightPos(-2.6f, 2.4f, 1.8f);*/
    float xRadius = -2.6f;
    float yRadius = 2.4f - 1.0f;
#pragma endregion
    //============================ ��Դ(end) ============================    

    
    //============================ ��Ⱦѭ��(start) ============================
#pragma region
    while (!glfwWindowShouldClose(window))
    {
        //����
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        
        // ��Ⱦָ��-���ƺڰ����ɫ
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::vec3 lightPos(abs(xRadius * sin(0.5 * currentFrame)), 1.0f + abs(yRadius * sin(1 * currentFrame)), 1.8f);
        //����cube
        shader.useShader();
        // ���ʹ�õ�view�ռ��µ�shader
        shader.setVec3("lightPos", lightPos);


        // ���ʹ�õ�model�ռ��µ�shader
        //shader.setVec3("viewPos", camera.m_Pos);
        //shader.setVec3("light.position", lightPos);
        
        shader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        shader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        shader.setFloat("material.shininess", specFactor);
        // test1: ���ù����ɫ�ǹ̶���
        //shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        //shader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f); // �����յ�����һЩ�Դ��䳡��
        //shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        // test2: ���ù����ɫ�Ƕ�̬�仯��
        glm::vec3 lightColor;
        lightColor.x = sin(glfwGetTime() * 2.0f);
        lightColor.y = sin(glfwGetTime() * 0.7f);
        lightColor.z = sin(glfwGetTime() * 1.3f);

        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // ����Ӱ��
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // �ܵ͵�Ӱ��

        shader.setVec3("light.ambient", ambientColor);
        shader.setVec3("light.diffuse", diffuseColor);
        shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        glm::mat4 model(1.0);
        glm::mat4 view = camera.GetCameraView();
        glm::mat4 projection = glm::perspective(glm::radians(camera.m_Fov), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        vertex.BindVertex();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        //����light
        shaderLight.useShader();
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        shaderLight.setMat4("model", model);
        shaderLight.setMat4("view", view);
        shaderLight.setMat4("projection", projection);
        vertex.BindVertex();
        glDrawArrays(GL_TRIANGLES, 0, 36);        
        
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
