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
#include <assimp/scene.h>=
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

//EBO�������ԣ���glDrawElements�ĵڶ����͵��ĸ��������õ�
struct IndicesElements
{
    unsigned int IndOffset;//����ƫ��
    unsigned int IndNum;// ������
    unsigned int materialId; // ���ʵ�ID��
};
vector<IndicesElements> indicesElement;//EBO��������


struct Material
{
    string diff;
    string spec;
    string norm;
};

vector<Material> materials; // ���ʵ�����
vector<string> textures;//��ͼ��·��
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
    
    // ��ӡmesh
    int meshNum = scene->mNumMeshes;
    //print(meshNum << endl);
    // ��ӡmesh������
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
        
        print("����������: " << mesh->mNumFaces * 3<< endl);
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
        //print("����IDƫ��:" << vID << endl);
        //print("��������ƫ��:" << indOffset << endl);
        //print("����ID: " << mesh->mMaterialIndex << endl);
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
    //��ӡ���ʵ�����
    //print(numMaterials);
    vector<aiTextureType> textureType = { aiTextureType_DIFFUSE , aiTextureType_SPECULAR ,aiTextureType_HEIGHT };
    vector<string> textureStr = { "diffuse: ", "specular: ", "normal: " };
    string* p;
    for (int i = 0; i < numMaterials; ++i)
    {
        print("��" << i << "������" << endl);
        aiMaterial* material = scene->mMaterials[i];
        aiString path;
        //print(material->GetName().C_Str());//��������
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
    // ��ͼȥ�ظ�
    vector<string>::iterator it;
    sort(textures.begin(), textures.end());
    it = unique(textures.begin(), textures.end());
    if (it != textures.end()) textures.erase(it, textures.end());
    print("��ͼ������: " << textures.size() << endl);
    for (int i = 0; i < textures.size(); ++i)
    {
        print("��" << i << "����ͼ��·��" << textures[i] << endl);
    }
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
    //============================ ��������(end) ============================


    //============================ ����(start) ============================
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
    //============================ ����(end) ============================    


    //============================ shader(start) ============================
#pragma region

    // model�ռ��µ�cube��phong shader�������������ֻ��������shader
    Shader shader("assets/vertex_multiLight.glsl", "assets/fragment_multiLight.glsl");
    // view�ռ��µ�cube��phong shader�����shaderû���޸�
    //Shader shader("assets/vertex_core_view.glsl", "assets/fragment_core_view.glsl");
    
    Shader shaderLight("assets/vertex_light.glsl", "assets/fragment_light.glsl");
    Shader AssimpModelShader("assets/vertex_model.glsl", "assets/fragment_model.glsl");
    Shader AssimpModelShader_Texture("assets/vertex_model_texture.glsl", "assets/fragment_model_texture.glsl");
#pragma endregion
    //============================ shader(end) ============================

    
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
        glm::mat4 model(1.0);
        glm::mat4 view = camera.GetCameraView();
        glm::mat4 projection = glm::perspective(glm::radians(camera.m_Fov), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        //����cube
        setMultiLightShader(shader);
        tex0.Activate(0);// ������������
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
        * Ҳ���Խ��зֿ�����-nanosuit
        for (int i = 0; i < indicesElement.size(); ++i)
            Draw(indicesElement[i], AssimpModelShader, view, projection);
        */
        /*
        //�����Ļ���-nanosuit����ʹ�ú���
        AssimpModelShader.setMat4("view", view);
        AssimpModelShader.setMat4("projection", projection);
        AssimpModel.BindVertex();
        glm::mat4 model(1.0);
        model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));
        AssimpModelShader.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(AssimpModel.GetIndicies().size()), GL_UNSIGNED_INT, 0);

        // ���mesh����-nanosuit��ʹ�ú���
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
        
        //����light
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
               
        
        //����ǰ��֡
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#pragma endregion
    //============================ ��Ⱦѭ��(end) ============================
    mesh.DeleteVertex();
    shader.deleteShaderPrograme();
    glfwTerminate();
    return 0;
}
