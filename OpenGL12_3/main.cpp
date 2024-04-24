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

// һ������һ�����
//Camera camera(glm::vec3(4.71751f, 1.68357f, -2.38664f),
//    glm::vec3(0.0f, 1.0f, 0.0f),
//    496.494f,
//    -14.05f);
// 10��������������
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
            {"Normal", 3},
            {"Texture", 2}
        });
    
    
#pragma endregion
    //============================ ��������(end) ============================


    //============================ ����(start) ============================
#pragma region
    Texture tex0("assets/container2.png", 0);
    Texture tex1("assets/container2_specular.png", 1);
#pragma endregion
    //============================ ����(end) ============================    


    //============================ shader(start) ============================
#pragma region

    // model�ռ��µ�cube��phong shader�������������ֻ��������shader
    Shader shader("assets/vertex_core.glsl", "assets/fragment_core.glsl");
    // view�ռ��µ�cube��phong shader�����shaderû���޸�
    //Shader shader("assets/vertex_core_view.glsl", "assets/fragment_core_view.glsl");
    
    Shader shaderLight("assets/vertex_light.glsl", "assets/fragment_light.glsl");

    shader.useShader();
    shader.setInt("material.diffuse", 0);
    shader.setInt("material.specular", 1);
#pragma endregion
    //============================ shader(end) ============================


    // 
    //============================ ��Դ(start) ============================
#pragma region
    glm::vec3 lightPos(2.6f, 0.4f, 1.0f);
    // ��������ã�����ע�͵�
    //shader.setVec3("light.direction", -0.2f, -1.0f, -0.3f);

    // ���Դ����
    shader.setFloat("light.constant", 1.0f);
    shader.setFloat("light.linear", 0.007f);
    shader.setFloat("light.quadratic", 0.0002f);
    shader.setVec3("light.position", camera.m_Pos);
    shader.setVec3("light.direction", camera.m_Front);
    shader.setFloat("light.cutOff", glm::cos(glm::radians(30.5f)));
    shader.setFloat("light.outerCutOff", glm::cos(glm::radians(35.5f)));
    /*float xRadius = -2.6f;
    float yRadius = 2.4f - 1.0f;*/
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
        
        //glm::vec3 lightPos(abs(xRadius * sin(0.5 * currentFrame)), 1.0f + abs(yRadius * sin(1 * currentFrame)), 1.8f);
        //����cube
        
        shader.useShader();
        // ���ʹ�õ�view�ռ��µ�shader
        //shader.setVec3("lightPos", lightPos);


        // ���ʹ�õ�model�ռ��µ�shader
        shader.setVec3("viewPos", camera.m_Pos);
        shader.setVec3("light.position", lightPos);
        
        shader.setFloat("material.shininess", 64.0);
        // test1: ���ù����ɫ�ǹ̶���
        shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        shader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f); // �����յ�����һЩ�Դ��䳡��
        shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        tex0.Activate(0);// ������������
        tex1.Activate(1);
        

        
        glm::mat4 view = camera.GetCameraView();
        glm::mat4 projection = glm::perspective(glm::radians(camera.m_Fov), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        //shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        vertex.BindVertex();
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model(1.0);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        //glDrawArrays(GL_TRIANGLES, 0, 36);
        
        //����light
        shaderLight.useShader();
        glm::mat4 model(1.0);
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