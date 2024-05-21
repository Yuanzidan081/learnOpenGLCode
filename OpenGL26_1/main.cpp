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
extern aiScene* scene;
extern Assimp::Importer importer;
const int screenWidth = 800;
const int screenHeight = 600;

int specFactor = 32;
bool firstMouse = true;
float lastX = (float)screenWidth / 2, lastY = (float)screenHeight / 2;
float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��
bool gamma = false;

template<typename type>
int GetUnitSize()
{
    return sizeof(type);
}


// 10��������������
Camera camera(glm::vec3(0.18f, 1.8f, 7.0f),
    glm::vec3(0.0, 1.0, 0.0), 
    -90.0f,-11.0f);
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

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        gamma = true;
    else
        gamma = false;
        
}


#pragma endregion
//============================ Function(End) ============================

int main()
{
    //============================ ��ʼ��(start) ============================
#pragma region
    GLFWwindow* window = GlInit(screenWidth, screenHeight, "LearnOpenGL");
    glfwWindowHint(GLFW_SAMPLES, 4);
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
            //std::cout << camera.m_Yaw << " " << camera.m_Pitch << " "
            //    << camera.m_Pos.x << " "  << camera.m_Pos.y  << " " << camera.m_Pos.z << std::endl;
        });
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
        {
            camera.MouseScroolEvent(yoffset);
        });

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

#pragma endregion
    //============================ ��ʼ��(end) ============================


    //============================ ��������(start) ============================
#pragma region
    

    // vertices��indices����Model�ļ�����
    // �����û�����VAO��VBO
    // ����InstanceVBO
    // ���û�����layout
    // ����InstanceVBO��layout��Ĭ��Instance�Ķ��������Ƿ��ڻ���layout�ĺ��棬����������layoutElements.size()

    Mesh floorMesh(planeVertices, sizeof(planeVertices));
    floorMesh.SetLayout(
        {
            {"position", 3, GetUnitSize<float>()},
            {"normal", 3, GetUnitSize<float>()},
            {"texture", 2, GetUnitSize<float>()},

        });
    Mesh lightMesh(lightVertices, sizeof(lightVertices));
    lightMesh.SetLayout(
        {
            {"position", 3, GetUnitSize<float>()}
        }
    );
#pragma endregion
    //============================ ��������(end) ============================



    //============================ shader(start) ============================
#pragma region


    // �򵥵�ʵ����
    Shader shaderGamma("assets/vertex_gamma_correction.glsl", "assets/fragment_gamma_correction.glsl");
    Shader shaderLightGamma("assets/vertex_light_gamma_correction.glsl", "assets/fragment_light_gamma_correction.glsl");
#pragma endregion
    //============================ shader(end) ============================

    //============================ ����(start) ============================
#pragma region

    Texture texFloorWithoutGamma("assets/texture1.jpg", 0, true, false);
    Texture texFloorWithGamma("assets/texture1.jpg", 0, true, true);


    

#pragma endregion
    //============================ ����(end) ============================


    
    //============================ ��Ⱦѭ��(start) ============================
#pragma region
    while (!glfwWindowShouldClose(window))
    {
        //����
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glm::mat4 model(1.0);
        glm::mat4 view = camera.GetCameraView();
        glm::mat4 projection = glm::perspective(glm::radians(camera.m_Fov), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);

        float lightZ = sin(0.5 * glfwGetTime()) + 1;
        // ��һ�׶�
        glEnable(GL_DEPTH_TEST);// ����cube�͵ƿ�����Ȳ���
        // ��Ⱦָ��-���ƺڰ����ɫ
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ���ڲ�ʹ��ģ�建��

        // ���Ƶذ��mesh
        shaderGamma.useShader();
        floorMesh.BindVertex();
        shaderGamma.setMat4("model", model);
        shaderGamma.setMat4("view", view);
        shaderGamma.setMat4("projection", projection);
        shaderGamma.setVec3("viewPos", camera.m_Pos);
        shaderGamma.setInt("gamma", gamma);
        if (gamma)
        {
            shaderGamma.setInt("floorTexture", 0);
            texFloorWithGamma.Activate();
        }
        else
        {
            shaderGamma.setInt("floorTexture", 0);
            texFloorWithoutGamma.Activate();
        }
        for (int i = 0; i < 4; ++i)
        {
            shaderGamma.setVec3(("lightPositions[" + std::to_string(i) + "]").c_str(), lightPositions[i] + glm::vec3(0.0f, 0.0f, lightZ));
            shaderGamma.setVec3(("lightColors[" + std::to_string(i) + "]").c_str(), lightColors[i]);
        }
            
        glDrawArrays(GL_TRIANGLES, 0, 6);
        floorMesh.UnBindVertex();

        // ���ƵƵ�mesh
        shaderLightGamma.useShader();
        lightMesh.BindVertex();
        
        shaderLightGamma.setMat4("view", view);
        shaderLightGamma.setMat4("projection", projection);
        shaderLightGamma.setInt("gamma", gamma);

        for (int i = 0; i < 4; ++i)
        {
            glm::mat4 model(1.0);
            
            model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
            model = glm::translate(model, lightPositions[i] + glm::vec3(0.0f, 0.0f, lightZ));
            shaderLightGamma.setMat4("model", model);
            shaderLightGamma.setVec3("lightColor", lightColors[i]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        lightMesh.UnBindVertex();
        //����ǰ��֡
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#pragma endregion
    //============================ ��Ⱦѭ��(end) ============================
    //houseMesh.DeleteVertex();

    //shaderHouse.deleteShaderPrograme();


    glfwTerminate();
    return 0;
}
