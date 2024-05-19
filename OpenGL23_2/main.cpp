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
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

template<typename type>
int GetUnitSize()
{
    return sizeof(type);
}


// 10个物体的相机设置
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
            //std::cout << camera.m_Yaw << " " << camera.m_Pitch << " "
            //    << camera.m_Pos.x << " "  << camera.m_Pos.y  << " " << camera.m_Pos.z << std::endl;
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
    unsigned int amount = 20000;
    std::vector<glm::mat4> modelMatrices(amount, glm::mat4(1.0));
    float radius = 25.0;
    float offset = 2.5f;
    for (unsigned int i = 0; i < amount; ++i)
    {
        glm::mat4 model(1.0);
        // 1. 位移，分布在半径为radius的圆形上，偏移的范围是[-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));


        // 缩放，在0.05-0.25之间缩放
        float scale = (rand() % 20) / 100.0f + 0.05;
        model = glm::scale(model, glm::vec3(scale));

        // 旋转:绕着一个随机选择的旋转轴向量随机旋转
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4, 0.6f, 0.8f));

        modelMatrices[i] = model;
    }

    // vertices和indices放在Model文件里了
    // 先设置基本的VAO和VBO
    // 设置InstanceVBO
    // 设置基本的layout
    // 设置InstanceVBO的layout，默认Instance的顶点属性是放在基本layout的后面，所以索引是layoutElements.size()
    
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
    // 设置完InstanceBuffer不要急着解绑，还要给Instance的顶点属性（这里是instanceMatrix）设置顶点偏移等属性
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
    //============================ 顶点数据(end) ============================



    //============================ shader(start) ============================
#pragma region


    // 简单的实例化
    //Shader shaderinstancing("assets/vertex_quad_instancing1.glsl", "assets/fragment_quad_instancing1.glsl");

    // 使用实例化数组
    //Shader shaderinstancing("assets/vertex_quad_instancing_array.glsl", "assets/fragment_quad_instancing_array.glsl");

    // 使用实例化数组的实例2
    //Shader shaderinstancing("assets/vertex_quad_instancing_array2.glsl", "assets/fragment_quad_instancing_array2.glsl");

    // 小行星带的建立，但还没有使用实例化
    Shader shaderModel("assets/vertex_model_planet.glsl", "assets/fragment_model_planet.glsl");
    // 小行星带的建立，使用实例化
    Shader shaderinstancingModel("assets/vertex_model_planet_instancing.glsl", "assets/fragment_model_planet_instancing.glsl");
#pragma endregion
    //============================ shader(end) ============================

    //============================ 纹理(start) ============================
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
    //============================ 纹理(end) ============================


    //============================ 实例化设置(start) ============================
#pragma region
    

    
#pragma endregion
    //============================ 实例化设置(end) ============================


    
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


        shaderModel.useShader();
        planetMesh.BindVertex();
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        planetMesh.DrawModel(shaderModel,model, view, projection);
        planetMesh.UnBindVertex();

        shaderinstancingModel.useShader();
        rockMesh.BindVertex();
        // 未使用实例化绘制陨石
        //for (unsigned int i = 0; i < amount; i++)
        //{
        //    //rockMesh.DrawModel(shaderinstancingModel, modelMatrices[i],view, projection);
        //}

        // 使用实例化绘制陨石
        rockMesh.DrawModelInstance(shaderinstancingModel, view, projection, amount);

        rockMesh.UnBindVertex();
        

        //交换前后帧
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#pragma endregion
    //============================ 渲染循环(end) ============================
    //houseMesh.DeleteVertex();

    //shaderHouse.deleteShaderPrograme();


    glfwTerminate();
    return 0;
}
