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
    Mesh mesh(vertices, sizeof(vertices));
    mesh.SetLayout(
        {
            {"Position", 3},
            {"Normal", 3},
            {"Texture", 2}
        }
    );
    Mesh skyboxMesh(skyboxVertices, sizeof(skyboxVertices));
    skyboxMesh.SetLayout(
        {
            {"Position", 3}
        }
    );
    Mesh AssimpModel = ImportModel("3Dresource/nanosuit_reflection/nanosuit.obj");
    AssimpModel.SetLayout({
        {"Position", 3},
        {"Normal", 3},
        {"Texture", 2}
        });
#pragma endregion
    //============================ 顶点数据(end) ============================



    //============================ shader(start) ============================
#pragma region

    // model空间下的cube的phong shader，本代码的设置只是针对这个shader
    //Shader shader("assets/vertex_multiLight.glsl", "assets/fragment_multiLight.glsl");
    // 模型反射天空盒效果的shader
    //Shader shader("assets/vertex_obj_sky_reflect.glsl", "assets/fragment_obj_sky_reflect.glsl");
    // 模型折射天空盒效果的shader
    Shader shader("assets/vertex_obj_sky_refract.glsl", "assets/fragment_obj_sky_refract.glsl");
    
    // 天空盒
    Shader shaderSkybox("assets/vertex_skybox.glsl", "assets/fragment_skybox.glsl"); 
    Shader AssimpModelShader("assets/vertex_model_texture.glsl", "assets/fragment_model_texture.glsl");
    Shader AssimpModelShader_Texture("assets/vertex_model_texture.glsl", "assets/fragment_model_texture_reflect.glsl");
    shader.useShader();
    shader.setInt("skybox", 0);
#pragma endregion
    //============================ shader(end) ============================

    //============================ 纹理(start) ============================
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
    
    // 天空盒数据
    std::vector<std::string> faces
    {
        "right.jpg",
        "left.jpg",
        "top.jpg",
        "bottom.jpg",
        "front.jpg",
        "back.jpg"
    };

    unsigned int cubemapTexture = loadCubeMap(faces, 0, "assets/skybox/");


#pragma endregion
    //============================ 纹理(end) ============================    
    
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
       
         // 常规绘制方法：绘制天空盒，作为第一个绘制的物体，初始的深度是无穷远，这样永远在其他渲染物体的后面
        /*glDepthMask(GL_FALSE);
        shaderSkybox.useShader();
        skyboxMesh.BindVertex();
        view = glm::mat4(glm::mat3(camera.GetCameraView()));
        shaderSkybox.setMat4("projection", projection);
        shaderSkybox.setMat4("view", view);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        shaderSkybox.setInt("skybox", 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        skyboxMesh.UnBindVertex();
        glDepthMask(GL_TRUE);
        view = camera.GetCameraView();*/

        
        //绘制cube
        shader.useShader();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("cameraPos", camera.m_Pos);
        mesh.BindVertex();

        // 地板
        
        //model = scale(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 10.0f));
        //shader.setMat4("model", model);
        //glDrawArrays(GL_TRIANGLES, 0, 36);


        // 立方体1

        model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, -1.1f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 立方体2
        model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 2.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        mesh.UnBindVertex();
        // AssimpModel模型
        AssimpModel.BindVertex();
        model = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.2, 0.2));
        model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
        shader.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(AssimpModel.GetIndicies().size()), GL_UNSIGNED_INT, 0);
        AssimpModel.UnBindVertex();

        // 原始模型贴图
        setModelShader_texture(AssimpModelShader);

        AssimpModel.BindVertex();
        for (int i = 0; i < modelTexture.size(); ++i)
            modelTexture[i].Activate(textureMap[textures[i]]);

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
        AssimpModel.UnBindVertex();


        // 立方体贴图+ 反射贴图
        setModelShader_texture(AssimpModelShader_Texture);

        AssimpModel.BindVertex();
        for (int i = 0; i < modelTexture.size(); ++i)
            modelTexture[i].Activate(textureMap[textures[i]]);

        glActiveTexture(GL_TEXTURE18);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        AssimpModelShader_Texture.setInt("skybox", 18);
        AssimpModelShader_Texture.setVec3("cameraPos", camera.m_Pos);
        for (int i = 0; i < indicesElement.size(); ++i)
        {
            int matID = indicesElement[i].materialId;
            int difftextureID = GetTextureID(textureMap, materials[matID].diff);
            int spectextureID = GetTextureID(textureMap, materials[matID].diff);
            int refltextureID = GetTextureID(textureMap, materials[matID].refl);
            //print(i << ": " << refltextureID << endl);
            AssimpModelShader_Texture.setInt("material.diffuse", difftextureID);
            AssimpModelShader_Texture.setInt("material.specular", spectextureID);
            AssimpModelShader_Texture.setInt("material.refl", refltextureID);
            Draw(indicesElement[i], AssimpModelShader_Texture, view, projection, glm::vec3(0.2f, 0.2f, 0.2f),
                glm::vec3(20.0f, 3.0f, 0.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        }
        AssimpModel.UnBindVertex();
        
        // early-z技术
        glDepthFunc(GL_LEQUAL);
        shaderSkybox.useShader();
        skyboxMesh.BindVertex();
        view = glm::mat4(glm::mat3(camera.GetCameraView()));
        shaderSkybox.setMat4("projection", projection);
        shaderSkybox.setMat4("view", view);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        shaderSkybox.setInt("skybox", 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        skyboxMesh.UnBindVertex();
        glDepthFunc(GL_LESS); // set depth function back to default
       


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
