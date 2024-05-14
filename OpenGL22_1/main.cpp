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
float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��


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
            /*std::cout << camera.m_Yaw << " " << camera.m_Pitch << " "
                << camera.m_Pos.x << " "  << camera.m_Pos.y  << " " << camera.m_Pos.z << std::endl;*/
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
    // vertices��indices����Model�ļ�����
    Mesh houseMesh(houseVertices, sizeof(houseVertices));
    houseMesh.SetLayout(
        {
                {"Position", 2},
                {"Color", 3},
        }
    );
    Mesh AssimpModel = ImportModel("3Dresource/nanosuit_reflection/nanosuit.obj");
    AssimpModel.SetLayout({
        {"Position", 3},
        {"Normal", 3},
        {"Texture", 2}
        });
#pragma endregion
    //============================ ��������(end) ============================



    //============================ shader(start) ============================
#pragma region

    // model�ռ��µ�cube��phong shader�������������ֻ��������shader
    //Shader shader("assets/vertex_multiLight.glsl", "assets/fragment_multiLight.glsl");

    Shader shaderHouse("assets/vertex_house.glsl", "assets/fragment_house.glsl", "assets/geometry_house.glsl");
    Shader AssimpModelShader("assets/vertex_model_texture.glsl", "assets/fragment_model_texture.glsl");
    Shader AssimpModelExplodeShader("assets/vertex_explode.glsl", "assets/fragment_explode.glsl", "assets/geometry_explode.glsl");
    Shader AssimpModelNormalVisualizationShader("assets/vertex_normal_visualization.glsl", "assets/fragment_normal_visualization.glsl", "assets/geometry_normal_visualization.glsl");
#pragma endregion
    //============================ shader(end) ============================

    //============================ ����(start) ============================
#pragma region
    std::unordered_map<std::string, int> textureMap;
    vector<Texture> modelTexture;
    std::cout << textures.size() << std::endl;
    for (int j = 0; j < textures.size(); ++j)
    {
        
        string path = "3Dresource/nanosuit_reflection/";
        modelTexture.push_back(Texture((path + textures[j]).c_str(), j, true));
        textureMap.insert({ textures[j], j });
    }
    




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

        
        glDepthMask(GL_TRUE);
         
        // ��һ�׶�
        glEnable(GL_DEPTH_TEST);// ����cube�͵ƿ�����Ȳ���
        // ��Ⱦָ��-���ƺڰ����ɫ
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ���ڲ�ʹ��ģ�建��
       

        
        
        //houseMesh.BindVertex();
        //shaderHouse.useShader();
        //glDrawArrays(GL_POINTS, 0, 4);// ÿһ����Ϊ��λ����һ������
        //houseMesh.UnBindVertex();

        

        //// ԭʼģ����ͼ
        setModelShader_texture(AssimpModelShader);

        AssimpModel.BindVertex();
        for (int i = 0; i < indicesElement.size(); ++i)
        {
            int matID = indicesElement[i].materialId;
            int difftextureID = GetTextureID(textureMap, materials[matID].diff);
            int spectextureID = GetTextureID(textureMap, materials[matID].diff);
            //print(i << ": " << textureID << endl);
            AssimpModelShader.setInt("material.diffuse", difftextureID);
            AssimpModelShader.setInt("material.specular", spectextureID);
            Draw(indicesElement[i], AssimpModelShader, view, projection, glm::vec3(0.2f, 0.2f, 0.2f),
                glm::vec3(10.0f, 3.0f, 0.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        }

        AssimpModelNormalVisualizationShader.useShader();
        for (int i = 0; i < indicesElement.size(); ++i)
        {
            int matID = indicesElement[i].materialId;
            int difftextureID = GetTextureID(textureMap, materials[matID].diff);
            int spectextureID = GetTextureID(textureMap, materials[matID].diff);
            //print(i << ": " << textureID << endl);
            Draw(indicesElement[i], AssimpModelNormalVisualizationShader, view, projection, glm::vec3(0.2f, 0.2f, 0.2f),
                glm::vec3(10.0f, 3.0f, 0.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        }
        AssimpModel.UnBindVertex();
        
        AssimpModelExplodeShader.useShader();
        AssimpModelExplodeShader.setFloat("time", glfwGetTime());
        AssimpModel.BindVertex();
        for (int i = 0; i < indicesElement.size(); ++i)
        {
            int matID = indicesElement[i].materialId;
            int difftextureID = GetTextureID(textureMap, materials[matID].diff);
            //print(i << ": " << textureID << endl);
            AssimpModelExplodeShader.setInt("texture_diffuse1", difftextureID);
            Draw(indicesElement[i], AssimpModelExplodeShader, view, projection, glm::vec3(0.2f, 0.2f, 0.2f),
                glm::vec3(0.0f, 3.0f, 0.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        }
        AssimpModel.UnBindVertex();

        //����ǰ��֡
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#pragma endregion
    //============================ ��Ⱦѭ��(end) ============================
    houseMesh.DeleteVertex();

    shaderHouse.deleteShaderPrograme();


    glfwTerminate();
    return 0;
}
