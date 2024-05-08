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
#include "Base.h"
#include "ShaderSetting.h"
#include <vector>
#include <map>
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
    Mesh mesh(vertices, sizeof(vertices));
    mesh.SetLayout(
        {
            {"Position", 3},
            {"Normal", 3},
            {"Texture", 2}
        });
    //�ı��ε�λ��
    Mesh quadMesh(quadVertices, sizeof(quadVertices));
    quadMesh.SetLayout(
        {
            {"Position", 2},
            {"Texture", 2}
        });

#pragma endregion
    //============================ ��������(end) ============================


    //============================ ����(start) ============================
#pragma region
    Texture tex0("assets/container2.png", 0);
    Texture tex1("assets/container2_specular.png", 1);
    Texture tex2("assets/texture1.jpg", 2);


#pragma endregion
    //============================ ����(end) ============================    
    //============================ shader(start) ============================
#pragma region

    // model�ռ��µ�cube��phong shader�������������ֻ��������shader
    Shader shader("assets/vertex_multiLight.glsl", "assets/fragment_multiLight.glsl");
    //Shader shaderFrameQuad("assets/vertex_quad.glsl", "assets/fragment_quad.glsl");// ��֮ǰ��ͬ��Ч��
    Shader shaderFrameQuad("assets/vertex_quad.glsl", "assets/fragment_quad_invert.glsl");// �����Ч��
    //Shader shaderFrameQuad("assets/vertex_quad.glsl", "assets/fragment_quad_gray.glsl");// �Ҷȵ�Ч��
    //Shader shaderFrameQuad("assets/vertex_quad.glsl", "assets/fragment_quad_sharpen.glsl");// �񻯵�Ч��
    //Shader shaderFrameQuad("assets/vertex_quad.glsl", "assets/fragment_quad_blur.glsl");// ģ����Ч��
    //Shader shaderFrameQuad("assets/vertex_quad.glsl", "assets/fragment_quad_edge_detection.glsl");// ģ����Ч��
    Shader shaderLight("assets/vertex_light.glsl", "assets/fragment_light.glsl");
    
#pragma endregion
    //============================ shader(end) ============================

//============================ ֡�������(start) ============================
#pragma region
    //// ����fbo
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // ����֡����������������
    unsigned int frameTexture;
    glGenTextures(1, &frameTexture);
    glBindTexture(GL_TEXTURE_2D, frameTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // �����ӵ�֡����
    ////                      1.֡������� 2.�������ͣ�0��ζ���ж����ɫ���� 3. �������� 4.������ 5.mipmap���𣬱���Ϊ0
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameTexture, 0);
    // ������Ⱦ�������
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    // ������Ⱥ�ģ����Ⱦ������� ��װ24λ����Ⱥ�8λ��ģ�建��
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // ������Ⱦ�������
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    
    // ע������Ҫ�����������ݣ�ʹ����Ⱦ���������Ҫ�����������ݣ�ѡ��������


    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

#pragma endregion
//============================ ֡�������(end) ============================


    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    //============================ ��Ⱦѭ��(start) ============================
#pragma region
    while (!glfwWindowShouldClose(window))
    {
        //����
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        
        
        // ��һ�׶�
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glEnable(GL_DEPTH_TEST);// ����cube�͵ƿ�����Ȳ���
        // ��Ⱦָ��-���ƺڰ����ɫ
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ���ڲ�ʹ��ģ�建��
        glm::mat4 model(1.0);
        glm::mat4 view = camera.GetCameraView();
        glm::mat4 projection = glm::perspective(glm::radians(camera.m_Fov), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        

        //����cube
        setMultiLightShader(shader);
        tex0.Activate(0);// ������������
        //shader.useShader();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        mesh.BindVertex();

        // �ذ�

        tex2.Activate(2);
        shader.setInt("material.diffuse", 2);
        shader.setInt("material.specular", 2);
        model = scale(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 10.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        

        // ������1

        tex0.Activate(0);
        shader.setInt("material.diffuse", 0);
        tex1.Activate(1);
        shader.setInt("material.specular", 1);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, -1.1f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // ������2
        model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 0.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        mesh.UnBindVertex();
        
        ////����light
        shaderLight.useShader();
        model = glm::mat4(1.0);
        mesh.BindVertex();
        
        shaderLight.setMat4("view", view);
        shaderLight.setMat4("projection", projection);
        for (int i = 0; i < 4; ++i)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            shaderLight.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        mesh.UnBindVertex();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);// ����Ĭ�ϵ�֡����

        glEnable(GL_DEPTH_TEST);// ����cube�͵ƿ�����Ȳ���
        // ��Ⱦָ��-���ƺڰ����ɫ
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ���ڲ�ʹ��ģ�建��


        //����cube
        setMultiLightShader(shader);
        tex0.Activate(0);// ������������
        //shader.useShader();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        mesh.BindVertex();

        // �ذ�

        tex2.Activate(2);
        shader.setInt("material.diffuse", 2);
        shader.setInt("material.specular", 2);
        model = scale(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 10.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);



        // ������1

        tex0.Activate(0);
        shader.setInt("material.diffuse", 0);
        tex1.Activate(1);
        shader.setInt("material.specular", 1);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, -1.1f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // ������2
        model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 0.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        mesh.UnBindVertex();

        ////����light
        shaderLight.useShader();
        model = glm::mat4(1.0);
        mesh.BindVertex();

        shaderLight.setMat4("view", view);
        shaderLight.setMat4("projection", projection);
        for (int i = 0; i < 4; ++i)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            shaderLight.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        mesh.UnBindVertex();

        glDisable(GL_DEPTH_TEST);
        shaderFrameQuad.useShader();
        quadMesh.BindVertex();
        //glActiveTexture(33984 + 5);
        /*glBindTexture(GL_TEXTURE_2D, frameTexture);*/
        glActiveTexture(GL_TEXTURE0);  // ��Ϊʹ���˶����ۣ�����Ҫ����ָ��ʹ�ò��
        glBindTexture(GL_TEXTURE_2D, frameTexture);  // ��FBO������
        shaderFrameQuad.setInt("screenTexture", 0);//�����shaderFrameQuadҪ���õú����漤��Ĳ��һ��
        glDrawArrays(GL_TRIANGLES, 0, 6);
        quadMesh.UnBindVertex();
        


        //����ǰ��֡
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#pragma endregion
    //============================ ��Ⱦѭ��(end) ============================
    mesh.DeleteVertex();
    quadMesh.DeleteVertex();
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
    shader.deleteShaderPrograme();
    shaderLight.deleteShaderPrograme();
    shaderFrameQuad.deleteShaderPrograme();

    glfwTerminate();
    return 0;
}
