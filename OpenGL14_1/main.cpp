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
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Camera.h"
#include "Base.h"
#include "ShaderSetting.h"
#include <unordered_map>
//============================ Function(Start) ============================
#pragma region

const int screenWidth = 800;
const int screenHeight = 600;

int specFactor = 32;
bool firstMouse = true;
float lastX = (float)screenWidth / 2, lastY = (float)screenHeight / 2;
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

// 一个物体一个相机
//Camera camera(glm::vec3(4.71751f, 1.68357f, -2.38664f),
//    glm::vec3(0.0f, 1.0f, 0.0f),
//    496.494f,
//    -14.05f);
// 10个物体的相机设置
Camera camera(glm::vec3(5.74329f, 2.40024f, 3.36758f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    587.843f,
    -15.5f);

//EBO绘制属性，在glDrawElements的第二个和第四个参数会用到
struct IndicesElements
{
    unsigned int IndOffset;//顶点偏移
    unsigned int IndNum;// 顶点数
    unsigned int materialId; // 材质的ID号
};
vector<IndicesElements> indicesElement;//EBO属性数组


struct Material
{
    string diff;
    string spec;
    string norm;
};

vector<Material> materials; // 材质的容器
vector<string> textures;//贴图的路径
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


int GetTextureID(unordered_map<string, int>& textureMap, string& textureStr)
{
   if (textureMap.find(textureStr) != textureMap.end())
        return textureMap[textureStr];
    return -1;
}
Mesh ImportModel(const std::string &path)
{
    // Create an instance of the Importer class
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);

    // If the import failed, report it
    if (!scene)
    {
        std::cout << "import error!" << std::endl;
        //return;
    }
    std::cout << "import success!" << std::endl;
    
    vector<Vertex> modelVertices;
    vector<unsigned int> modelIndices;
    
    // 打印mesh
    int meshNum = scene->mNumMeshes;
    //print(meshNum << endl);
    // 打印mesh的名字
    int vID = 0;
    int indOffset = 0;
    for (int i = 0; i < meshNum; ++i)
    {
        aiMesh* mesh = scene->mMeshes[i];
        //print(mesh->mName.C_Str() << ":");
        int numVertex = mesh->mNumVertices;
        //print(numVertex << endl);
        for (int j = 0; j < numVertex; ++j)
        {
            Vertex vTemp;
            vTemp.Position = convertVec3(mesh->mVertices[j]);
            vTemp.Normal = convertVec3(mesh->mNormals[j]);
            vTemp.TexCoords = convertVec2(mesh->mTextureCoords[0][j]);
            modelVertices.push_back(vTemp);
        }
        
        print("顶点索引数: " << mesh->mNumFaces * 3<< endl);
        for (int j = 0; j < mesh->mNumFaces; ++j)
        {
            aiFace faces = mesh->mFaces[j];
            //cout << "(";
            for (int k = 0; k < faces.mNumIndices; ++k)
            {
                modelIndices.push_back(faces.mIndices[k] + vID);
                //cout << faces.mIndices[k] + vID << ", ";
            }
            //cout << ")" << endl;
        }
        //print("顶点ID偏移:" << vID << endl);
        //print("顶点索引偏移:" << indOffset << endl);
        //print("材质ID: " << mesh->mMaterialIndex << endl);
        //print("=======================" << endl);
        IndicesElements tempIE;
        tempIE.IndNum = mesh->mNumFaces * 3;
        tempIE.IndOffset = indOffset;
        tempIE.materialId = mesh->mMaterialIndex;
        indicesElement.push_back(tempIE);
        vID += mesh->mNumVertices;
        indOffset += mesh->mNumFaces * 3;


        
    }
    //print(modelVertices.size() << endl);
    //print(modelIndices.size() << endl);
    
    int numMaterials = scene->mNumMaterials;
    //打印材质的数量
    //print(numMaterials);
    vector<aiTextureType> textureType = { aiTextureType_DIFFUSE , aiTextureType_SPECULAR ,aiTextureType_HEIGHT };
    vector<string> textureStr = { "diffuse: ", "specular: ", "normal: " };
    string* p;
    for (int i = 0; i < numMaterials; ++i)
    {
        //print("第" << i << "个材料" << endl);
        aiMaterial* material = scene->mMaterials[i];
        aiString path;
        //print(material->GetName().C_Str());//材质名字
        Material tempMat;
        p = &tempMat.diff;
        for (int k = 0; k < textureType.size(); ++k)
        {
            
            material->GetTexture(textureType[k], 0, &path);
            string texturePath = path.C_Str();
            //print(textureStr[k] <<  texturePath << endl);

            string pathRel;
            if (texturePath.find_first_of("\\") != std::string::npos)
                pathRel = texturePath.substr(texturePath.find_last_of("\\") + 1);
            else
                pathRel = texturePath;
            if (pathRel.size() > 0)
            {
                (*p++) = pathRel;
                textures.push_back(pathRel);
            }
            else
            {
                (*p++) = "";
            }
                
        }
        materials.push_back(tempMat);
        
       
    }
    // 贴图去重复
    vector<string>::iterator it;
    sort(textures.begin(), textures.end());
    it = unique(textures.begin(), textures.end());
    if (it != textures.end()) textures.erase(it, textures.end());
    //print("贴图的数量: " << textures.size() << endl);
    /*for (int i = 0; i < textures.size(); ++i)
    {
        print("第" << i << "张贴图的路径" << textures[i] << endl);
    }*/
    return Mesh(modelVertices, modelIndices);

    
    
}

void Draw(Mesh& mesh, Shader& shader,
    glm::mat4 V,  glm::mat4 P, glm::vec3 scaleSize = glm::vec3(1, 1, 1), glm::vec3 translate = glm::vec3(0, 0, 0),
    float angle = 0, glm::vec3 axis = glm::vec3(1, 0, 0))
{
    glm::mat4 M(1);
    M = glm::translate(M, translate);
    M = glm::rotate(M, glm::radians(angle), axis);
    M = glm::scale(M, scaleSize);

    shader.setMat4("model", M);
    shader.setMat4("view", V);
    shader.setMat4("projection", P);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.GetIndicies().size()), GL_UNSIGNED_INT, 0);
}

void Draw(IndicesElements& IE, Shader& shader,
    glm::mat4 V, glm::mat4 P, glm::vec3 scaleSize = glm::vec3(1, 1, 1), glm::vec3 translate = glm::vec3(0, 0, 0),
    float angle = 0, glm::vec3 axis = glm::vec3(1, 0, 0))
{
    glm::mat4 M(1);
    M = glm::translate(M, translate);
    M = glm::rotate(M, glm::radians(angle), axis);
    M = glm::scale(M, scaleSize);

    shader.setMat4("model", M);
    shader.setMat4("view", V);
    shader.setMat4("projection", P);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(IE.IndNum), GL_UNSIGNED_INT, (void*)(IE.IndOffset * sizeof(float)));
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
    //nanosuit
    //Mesh AssimpModel = ImportModel("3Dresource/nanosuit/nanosuit.obj");
    //girl
    //Mesh AssimpModel = ImportModel("3Dresource/girl/untitled-scene.3dcool.net.obj");
    //apple
    Mesh AssimpModel = ImportModel("3Dresource/ONI/ONI.obj");
    AssimpModel.SetLayout({
        {"Position", 3},
        {"Normal", 3},
        {"Texture", 2}
        });
#pragma endregion
    //============================ 顶点数据(end) ============================


    //============================ 纹理(start) ============================
#pragma region
    std::unordered_map<std::string, int> textureMap;
    Texture tex0("assets/container2.png", 0);
    textureMap.insert({ "assets/container2.png", 0 });
    Texture tex1("assets/container2_specular.png", 1);
    textureMap.insert({ "assets/container2_specular.png", 1 });
    vector<Texture> modelTexture;
    int i = 2;
    for (int j = 0; j < textures.size(); ++j, ++i)
    {
        string path = "3Dresource/ONI/";
        modelTexture.push_back(Texture((path + textures[j]).c_str(), i));
        textureMap.insert({ textures[j], i });
    }
#pragma endregion
    //============================ 纹理(end) ============================    


    //============================ shader(start) ============================
#pragma region

    // model空间下的cube的phong shader，本代码的设置只是针对这个shader
    Shader shader("assets/vertex_multiLight.glsl", "assets/fragment_multiLight.glsl");
    // view空间下的cube的phong shader，这个shader没有修改
    //Shader shader("assets/vertex_core_view.glsl", "assets/fragment_core_view.glsl");
    
    Shader shaderLight("assets/vertex_light.glsl", "assets/fragment_light.glsl");
    Shader AssimpModelShader("assets/vertex_model.glsl", "assets/fragment_model.glsl");
    Shader AssimpModelShader_Texture("assets/vertex_model_texture.glsl", "assets/fragment_model_texture.glsl");
#pragma endregion
    //============================ shader(end) ============================

    
    //============================ 渲染循环(start) ============================
#pragma region
    while (!glfwWindowShouldClose(window))
    {
        //输入
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        
        // 渲染指令-类似黑板的颜色
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 model(1.0);
        glm::mat4 view = camera.GetCameraView();
        glm::mat4 projection = glm::perspective(glm::radians(camera.m_Fov), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        //绘制cube
        setMultiLightShader(shader);
        tex0.Activate(0);// 激活纹理坐标
        tex1.Activate(1);
        //
        //
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        mesh.BindVertex();
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model(1.0);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        mesh.UnBindVertex();

        setModelShader(AssimpModelShader);
        AssimpModel.BindVertex();

        //nanosuit
        //Draw(AssimpModel, AssimpModelShader, view, projection, glm::vec3(0.2, 0.2, 0.2));
        for (int i = 0; i < indicesElement.size(); ++i)
        {
            Draw(indicesElement[i], AssimpModelShader, view, projection, glm::vec3(1, 1, 1),
                glm::vec3(0.0f, 0.0f, 0.0f), 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        }
            
        //girl 
        //Draw(AssimpModel, AssimpModelShader, view, projection, glm::vec3(0.01,0.01,0.01));
        /*
        * 也可以进行分开绘制-nanosuit
        for (int i = 0; i < indicesElement.size(); ++i)
            Draw(indicesElement[i], AssimpModelShader, view, projection);
        */
        /*
        //完整的绘制-nanosuit，不使用函数
        AssimpModelShader.setMat4("view", view);
        AssimpModelShader.setMat4("projection", projection);
        AssimpModel.BindVertex();
        glm::mat4 model(1.0);
        model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));
        AssimpModelShader.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(AssimpModel.GetIndicies().size()), GL_UNSIGNED_INT, 0);

        // 逐个mesh绘制-nanosuit，使用函数
        //// visor
        //glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(156), GL_UNSIGNED_INT, 0);
        //// legs
        //glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(15222), GL_UNSIGNED_INT, (void*)(156 * sizeof(float)));
        //// hands
        //glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(19350), GL_UNSIGNED_INT, (void*)(15378 * sizeof(float)));
        ////lights
        //glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(78), GL_UNSIGNED_INT, (void*)(34728 * sizeof(float)));
        ////arms
        //glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(6804), GL_UNSIGNED_INT, (void*)(34806 * sizeof(float)));
        //// helmets
        //glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(7248), GL_UNSIGNED_INT, (void*)(41610 * sizeof(float)));
        //// body
        //glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(8316), GL_UNSIGNED_INT, (void*)(48858 * sizeof(float)));
        */
        
        AssimpModel.UnBindVertex();

        setModelShader_texture(AssimpModelShader_Texture);
        
        AssimpModel.BindVertex();
        for (int i = 0; i < modelTexture.size(); ++i)
            modelTexture[i].Activate(textureMap[textures[i]]);

        for (int i = 0; i < indicesElement.size(); ++i)
        {
            int matID = indicesElement[i].materialId;
            int difftextureID = GetTextureID(textureMap, materials[matID].diff);
            int spectextureID = GetTextureID(textureMap, materials[matID].diff);
            //print(i << ": " << textureID << endl);
            AssimpModelShader_Texture.setInt("material.diffuse", difftextureID);
            AssimpModelShader_Texture.setInt("material.specular", spectextureID);
            Draw(indicesElement[i], AssimpModelShader_Texture, view, projection, glm::vec3(1, 1, 1),
                glm::vec3(3.5, 0, 0),180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        AssimpModel.UnBindVertex();
        //glDrawArrays(GL_TRIANGLES, 0, 36);
        
        //绘制light
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
