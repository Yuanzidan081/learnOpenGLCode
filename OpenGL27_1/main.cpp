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
bool usePCF = false;
template<typename type>
int GetUnitSize()
{
    return sizeof(type);
}
glm::vec3 lightPosition(0.0f, 4.0f, 10.0f);

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


    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        lightPosition.x += 0.1;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        lightPosition.x -= 0.1;
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        lightPosition.y += 0.1;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        lightPosition.y -= 0.1;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        lightPosition.z += 0.1;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        lightPosition.z -= 0.1;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        usePCF = true;
    else
        usePCF = false;
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
    glEnable(GL_CULL_FACE);

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
            {"texture", 2, GetUnitSize<float>()}

        });
    Mesh lightMesh(lightVertices, sizeof(lightVertices));
    lightMesh.SetLayout(
        {
            {"position", 3, GetUnitSize<float>()}
        }
    );
    Mesh quadMesh(quadVertices, sizeof(quadVertices));
    quadMesh.SetLayout(
        {
                {"position", 3, GetUnitSize<float>()},
                {"texture", 2, GetUnitSize<float>()}
        });
    ImportModel("3Dresource/nanosuit/nanosuit.obj");
    tuple<vector<Vertex>, vector<unsigned int>> nanosuitInfo = GetScenceBasicInfo();
    Mesh nanosuitMesh(std::get<0>(nanosuitInfo), std::get<1>(nanosuitInfo), true);
    //
    setImportMeshProp(nanosuitMesh);
    nanosuitMesh.SetLayout({
        {"Position", 3,GetUnitSize<float>()},
        {"Normal", 3,GetUnitSize<float>()},
        {"Texture", 2,GetUnitSize<float>()}
        });
    

#pragma endregion
    //============================ ��������(end) ============================



    //============================ shader(start) ============================
#pragma region


    // �򵥵�ʵ����
    Shader shader_blinnphong("assets/vertex_shadow_blinnphong.glsl", "assets/fragment_shadow_blinnphong.glsl");
    Shader shader_shadowmap("assets/vertex_shadowmap.glsl", "assets/fragment_shadowmap.glsl");
    Shader shader_shadowmap_quad("assets/vertex_shadowmap_quad.glsl", "assets/fragment_shadowmap_quad.glsl");
    Shader shader_light("assets/vertex_light.glsl", "assets/fragment_light.glsl");

    shader_shadowmap_quad.useShader();
    shader_shadowmap_quad.setInt("depthMap", 31);
#pragma endregion
    //============================ shader(end) ============================

    //============================ ����(start) ============================
#pragma region

    //Texture texFloorWithoutGamma("assets/texture1.jpg", 0, true, false);
    Texture texFloorWithGamma("assets/texture1.jpg", 1, true, true);

    nanosuitMesh.SetModelTextureMap("3Dresource/nanosuit/");
    

#pragma endregion
    //============================ ����(end) ============================

     //============================ Shadow map(start) ============================
#pragma region
    //����һ��֡�������
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // ����2D����
    const GLuint shadow_width = 1024, shadow_height = 1024;
    // create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glActiveTexture(GL_TEXTURE31);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    



#pragma endregion
    //============================ Shadow map(end) ============================


    //============================ ��Ⱦѭ��(start) ============================
#pragma region
    while (!glfwWindowShouldClose(window))
    {
        glm::mat4 model(1.0);
        glm::mat4 view = camera.GetCameraView();
        glm::mat4 projection = glm::perspective(glm::radians(camera.m_Fov), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);

        //����
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        //1. ����Ⱦ�����ͼ
        ////���������ͼ
        glViewport(0, 0, shadow_width, shadow_height);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);// ����cube�͵ƿ�����Ȳ���
        //����shader�ľ���
        GLfloat near_plane = 1.0f, far_plane = 50.0f;
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f,
            -10.0f, 10.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        
        shader_shadowmap.useShader();
        shader_shadowmap.setMat4("lightSapceMatrix", lightSpaceMatrix);

        

        nanosuitMesh.BindVertex();
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        glCullFace(GL_FRONT);
        nanosuitMesh.DrawModel(shader_shadowmap, model);
        glCullFace(GL_BACK); // ��Ҫ�������ԭ�ȵ�culling face
        nanosuitMesh.UnBindVertex();
        floorMesh.BindVertex();
        model = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 10.0f));
        shader_shadowmap.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        floorMesh.UnBindVertex();
        

        // �󶨻�Ĭ�ϵ�֡����
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        //���Ƴ���
        //// ��Ⱦָ��-���ƺڰ����ɫ
        glViewport(0, 0, screenWidth, screenHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ���ڲ�ʹ��ģ�建��

        lightMesh.BindVertex();
        shader_light.useShader();
        model = glm::translate(glm::mat4(1.0f), lightPosition);
        model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));
        shader_light.setMat4("model", model);
        shader_light.setMat4("view", view);
        shader_light.setMat4("projection", projection);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        lightMesh.UnBindVertex();


        shader_blinnphong.useShader();
        shader_blinnphong.setBool("usePCF", usePCF);
        nanosuitMesh.BindVertex();
        shader_blinnphong.setInt("shadowMap", 31);
        shader_blinnphong.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        shader_blinnphong.setVec3("lightPos", lightPosition);
        shader_blinnphong.setVec3("viewPos", camera.m_Pos);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        nanosuitMesh.DrawModel(shader_blinnphong, model, view, projection);
        nanosuitMesh.UnBindVertex();
        floorMesh.BindVertex();
        texFloorWithGamma.Activate();
        model = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 10.0f));
        shader_blinnphong.setMat4("model", model);
        shader_blinnphong.setInt("texture_diffuse1", texFloorWithGamma.GetGLTextureID());
        glDrawArrays(GL_TRIANGLES, 0, 6);
        floorMesh.UnBindVertex();

        
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
