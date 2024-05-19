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

template<typename type>
int GetUnitSize()
{
    return sizeof(type);
}


// 10��������������
Camera camera(glm::vec3(36.0f, 36.40024f, 36.36758f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    584.000f,
    -37.7f);
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
            //std::cout << camera.m_Yaw << " " << camera.m_Pitch << " "
            //    << camera.m_Pos.x << " "  << camera.m_Pos.y  << " " << camera.m_Pos.z << std::endl;
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
    unsigned int amount = 20000;
    std::vector<glm::mat4> modelMatrices(amount, glm::mat4(1.0));
    float radius = 25.0;
    float offset = 2.5f;
    for (unsigned int i = 0; i < amount; ++i)
    {
        glm::mat4 model(1.0);
        // 1. λ�ƣ��ֲ��ڰ뾶Ϊradius��Բ���ϣ�ƫ�Ƶķ�Χ��[-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));


        // ���ţ���0.05-0.25֮������
        float scale = (rand() % 20) / 100.0f + 0.05;
        model = glm::scale(model, glm::vec3(scale));

        // ��ת:����һ�����ѡ�����ת�����������ת
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4, 0.6f, 0.8f));

        modelMatrices[i] = model;
    }

    // vertices��indices����Model�ļ�����
    // �����û�����VAO��VBO
    // ����InstanceVBO
    // ���û�����layout
    // ����InstanceVBO��layout��Ĭ��Instance�Ķ��������Ƿ��ڻ���layout�ĺ��棬����������layoutElements.size()
    
    ImportModel("3Dresource/rock/rock.obj");
    tuple<vector<Vertex>, vector<unsigned int>> rockInfo = GetScenceBasicInfo();
    Mesh rockMesh(std::get<0>(rockInfo), std::get<1>(rockInfo), true);
    //
    setImportMeshProp(rockMesh);
    rockMesh.SetLayout({
        {"Position", 3,GetUnitSize<float>()},
        {"Normal", 3,GetUnitSize<float>()},
        {"Texture", 2,GetUnitSize<float>()}
        });
    // ������InstanceBuffer��Ҫ���Ž�󣬻�Ҫ��Instance�Ķ������ԣ�������instanceMatrix�����ö���ƫ�Ƶ�����
    rockMesh.setInstanceMat4(&(modelMatrices[0]), sizeof(glm::mat4) * amount);
    rockMesh.SetInstanceLayout(
        {
            {"instanceMatCol1", 4, GetUnitSize<glm::vec4>()},
            {"instanceMatCol2", 4, GetUnitSize<glm::vec4>()},
            {"instanceMatCol4", 4, GetUnitSize<glm::vec4>()},
            {"instanceMatCol3", 4, GetUnitSize<glm::vec4>()}
        });
   


    ImportModel("3Dresource/planet/planet.obj");
    tuple<vector<Vertex>, vector<unsigned int>> planetInfo = GetScenceBasicInfo();
    Mesh planetMesh(std::get<0>(planetInfo), std::get<1>(planetInfo));
    setImportMeshProp(planetMesh);
    planetMesh.SetLayout({
        {"Position", 3, GetUnitSize<float>()},
        {"Normal", 3, GetUnitSize<float>()},
        {"Texture", 2, GetUnitSize<float>()}
        });
#pragma endregion
    //============================ ��������(end) ============================



    //============================ shader(start) ============================
#pragma region


    // �򵥵�ʵ����
    //Shader shaderinstancing("assets/vertex_quad_instancing1.glsl", "assets/fragment_quad_instancing1.glsl");

    // ʹ��ʵ��������
    //Shader shaderinstancing("assets/vertex_quad_instancing_array.glsl", "assets/fragment_quad_instancing_array.glsl");

    // ʹ��ʵ���������ʵ��2
    //Shader shaderinstancing("assets/vertex_quad_instancing_array2.glsl", "assets/fragment_quad_instancing_array2.glsl");

    // С���Ǵ��Ľ���������û��ʹ��ʵ����
    Shader shaderModel("assets/vertex_model_planet.glsl", "assets/fragment_model_planet.glsl");
    // С���Ǵ��Ľ�����ʹ��ʵ����
    Shader shaderinstancingModel("assets/vertex_model_planet_instancing.glsl", "assets/fragment_model_planet_instancing.glsl");
#pragma endregion
    //============================ shader(end) ============================

    //============================ ����(start) ============================
#pragma region
    //std::unordered_map<std::string, int> textureMap;
    //vector<Texture> modelTexture;
    //std::cout << textures.size() << std::endl;
    //for (int j = 0; j < textures.size(); ++j)
    //{
    //    
    //    string path = "3Dresource/nanosuit_reflection/";
    //    modelTexture.push_back(Texture((path + textures[j]).c_str(), j, true));
    //    textureMap.insert({ textures[j], j });
    //}
    planetMesh.SetModelTextureMap("3Dresource/planet/");
    rockMesh.SetModelTextureMap("3Dresource/rock/");

    


#pragma endregion
    //============================ ����(end) ============================


    //============================ ʵ��������(start) ============================
#pragma region
    

    
#pragma endregion
    //============================ ʵ��������(end) ============================


    
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


        shaderModel.useShader();
        planetMesh.BindVertex();
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        planetMesh.DrawModel(shaderModel,model, view, projection);
        planetMesh.UnBindVertex();

        shaderinstancingModel.useShader();
        rockMesh.BindVertex();
        // δʹ��ʵ����������ʯ
        //for (unsigned int i = 0; i < amount; i++)
        //{
        //    //rockMesh.DrawModel(shaderinstancingModel, modelMatrices[i],view, projection);
        //}

        // ʹ��ʵ����������ʯ
        rockMesh.DrawModelInstance(shaderinstancingModel, view, projection, amount);

        rockMesh.UnBindVertex();
        

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
