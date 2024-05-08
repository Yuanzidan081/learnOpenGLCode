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
        });
    //四边形的位置
    Mesh quadMesh(quadVertices, sizeof(quadVertices));
    quadMesh.SetLayout(
        {
            {"Position", 2},
            {"Texture", 2}
        });

#pragma endregion
    //============================ 顶点数据(end) ============================


    //============================ 纹理(start) ============================
#pragma region
    Texture tex0("assets/container2.png", 0);
    Texture tex1("assets/container2_specular.png", 1);
    Texture tex2("assets/texture1.jpg", 2);


#pragma endregion
    //============================ 纹理(end) ============================    
    //============================ shader(start) ============================
#pragma region

    // model空间下的cube的phong shader，本代码的设置只是针对这个shader
    Shader shader("assets/vertex_multiLight.glsl", "assets/fragment_multiLight.glsl");
    //Shader shaderFrameQuad("assets/vertex_quad.glsl", "assets/fragment_quad.glsl");// 和之前相同的效果
    Shader shaderFrameQuad("assets/vertex_quad.glsl", "assets/fragment_quad_invert.glsl");// 反相的效果
    //Shader shaderFrameQuad("assets/vertex_quad.glsl", "assets/fragment_quad_gray.glsl");// 灰度的效果
    //Shader shaderFrameQuad("assets/vertex_quad.glsl", "assets/fragment_quad_sharpen.glsl");// 锐化的效果
    //Shader shaderFrameQuad("assets/vertex_quad.glsl", "assets/fragment_quad_blur.glsl");// 模糊的效果
    //Shader shaderFrameQuad("assets/vertex_quad.glsl", "assets/fragment_quad_edge_detection.glsl");// 模糊的效果
    Shader shaderLight("assets/vertex_light.glsl", "assets/fragment_light.glsl");
    
#pragma endregion
    //============================ shader(end) ============================

//============================ 帧缓冲对象(start) ============================
#pragma region
    //// 创建fbo
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // 创建帧缓冲纹理并生成纹理
    unsigned int frameTexture;
    glGenTextures(1, &frameTexture);
    glBindTexture(GL_TEXTURE_2D, frameTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 纹理附加到帧缓冲
    ////                      1.帧缓冲对象 2.附件类型，0意味着有多个颜色附件 3. 纹理类型 4.纹理本身 5.mipmap级别，保留为0
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameTexture, 0);
    // 创建渲染缓冲对象
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    // 创建深度和模版渲染缓冲对象， 封装24位的深度和8位的模板缓冲
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // 附加渲染缓冲对象
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    
    // 注：不需要采样缓冲数据，使用渲染缓冲对象。需要采样缓冲数据，选择纹理附件


    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

#pragma endregion
//============================ 帧缓冲对象(end) ============================


    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    //============================ 渲染循环(start) ============================
#pragma region
    while (!glfwWindowShouldClose(window))
    {
        //输入
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        
        
        // 第一阶段
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glEnable(GL_DEPTH_TEST);// 绘制cube和灯开启深度测试
        // 渲染指令-类似黑板的颜色
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 现在不使用模板缓冲
        glm::mat4 model(1.0);
        glm::mat4 view = camera.GetCameraView();
        glm::mat4 projection = glm::perspective(glm::radians(camera.m_Fov), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        

        //绘制cube
        setMultiLightShader(shader);
        tex0.Activate(0);// 激活纹理坐标
        //shader.useShader();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        mesh.BindVertex();

        // 地板

        tex2.Activate(2);
        shader.setInt("material.diffuse", 2);
        shader.setInt("material.specular", 2);
        model = scale(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 10.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        

        // 立方体1

        tex0.Activate(0);
        shader.setInt("material.diffuse", 0);
        tex1.Activate(1);
        shader.setInt("material.specular", 1);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, -1.1f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 立方体2
        model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 0.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        mesh.UnBindVertex();
        
        ////绘制light
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

        glBindFramebuffer(GL_FRAMEBUFFER, 0);// 返回默认的帧缓冲

        glEnable(GL_DEPTH_TEST);// 绘制cube和灯开启深度测试
        // 渲染指令-类似黑板的颜色
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 现在不使用模板缓冲


        //绘制cube
        setMultiLightShader(shader);
        tex0.Activate(0);// 激活纹理坐标
        //shader.useShader();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        mesh.BindVertex();

        // 地板

        tex2.Activate(2);
        shader.setInt("material.diffuse", 2);
        shader.setInt("material.specular", 2);
        model = scale(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 10.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);



        // 立方体1

        tex0.Activate(0);
        shader.setInt("material.diffuse", 0);
        tex1.Activate(1);
        shader.setInt("material.specular", 1);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, -1.1f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 立方体2
        model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 0.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        mesh.UnBindVertex();

        ////绘制light
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
        glActiveTexture(GL_TEXTURE0);  // 因为使用了多个插槽，所以要重新指定使用插槽
        glBindTexture(GL_TEXTURE_2D, frameTexture);  // 绑定FBO的纹理
        shaderFrameQuad.setInt("screenTexture", 0);//这里的shaderFrameQuad要设置得和上面激活的插槽一致
        glDrawArrays(GL_TRIANGLES, 0, 6);
        quadMesh.UnBindVertex();
        


        //交换前后帧
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#pragma endregion
    //============================ 渲染循环(end) ============================
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
