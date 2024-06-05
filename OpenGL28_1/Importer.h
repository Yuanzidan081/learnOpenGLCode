#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <unordered_map>
#include <iostream>
#include "Vertex.h"
#include <string>
#include "Base.h"
#include "Shader.h"
using namespace std;
//EBO绘制属性，在glDrawElements的第二个和第四个参数会用到

//std::vector<IndicesElements> indicesElement;//EBO属性数组
//std::vector<Material> materials; // 材质的容器
//std::vector<std::string> textures;//贴图的路径

aiScene* scene = nullptr;
Assimp::Importer importer;
void ImportModel(const std::string& path)
{

    // Create an instance of the Importer class
    
    scene = const_cast<aiScene*>(importer.ReadFile(path,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType));

    // If the import failed, report it
    if (!scene)
    {
        std::cout << "import error!" << std::endl;
        //return;
    }
    std::cout << "import success!" << std::endl;
    int meshNum = scene->mNumMeshes;
    //std::shared_ptr<const aiScene> sceneData;
    //sceneData.reset(scene);
    //return sceneData;
    
    
    
}

tuple<vector<Vertex>, vector<unsigned int>> GetScenceBasicInfo()
{
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
            vTemp.Tangent = convertVec3(mesh->mTangents[j]);
            modelVertices.push_back(vTemp);
            //print3D(vTemp.Position);
            //print3D(vTemp.Normal);
            //print2D(vTemp.TexCoords);
            //cout << endl;
        }

        //print("顶点索引数: " << mesh->mNumFaces * 3 << endl);
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
        vID += mesh->mNumVertices;
        indOffset += mesh->mNumFaces * 3;
    }
    return { modelVertices , modelIndices };
}
void setImportMeshProp(Mesh& curMesh)
{
    //curMesh = Mesh(modelVertices, modelIndices);
    int meshNum = scene->mNumMeshes;
    int vID = 0;
    int indOffset = 0;
    for (int i = 0; i < meshNum; ++i)
    {
        aiMesh* mesh = scene->mMeshes[i];
        //print("顶点ID偏移:" << vID << endl);
        //print("顶点索引偏移:" << indOffset << endl);
        //print("材质ID: " << mesh->mMaterialIndex << endl);
        //print("=======================" << endl);
        IndicesElements tempIE;
        tempIE.IndNum = mesh->mNumFaces * 3;
        tempIE.IndOffset = indOffset;
        tempIE.materialId = mesh->mMaterialIndex;
        curMesh.GetIndicesElements().push_back(tempIE);
        vID += mesh->mNumVertices;
        indOffset += mesh->mNumFaces * 3;



    }
    //print(modelVertices.size() << endl);
    //print(modelIndices.size() << endl);

    int numMaterials = scene->mNumMaterials;
    //打印材质的数量
    //print(numMaterials);
    vector<aiTextureType> textureType = { aiTextureType_DIFFUSE , aiTextureType_SPECULAR ,aiTextureType_AMBIENT };
    vector<string> textureStr = { "diffuse: ", "specular: ", "reflect: " };
    string* p;
    for (int i = 0; i < numMaterials; ++i)
    {
        // print("第" << i << "个材料" << endl);
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
                curMesh.GetTextures().push_back(pathRel);
            }
            else
            {
                (*p++) = "lightBrown.png";
            }
        }
        curMesh.GetMaterial().push_back(tempMat);


    }
    curMesh.SetUniqueTexture();
    // 贴图去重复
    /*vector<string>::iterator it;
    sort(curMesh.GetTextures().begin(), curMesh.GetTextures().end());
    it = unique(curMesh.GetTextures().begin(), curMesh.GetTextures().end());
    if (it != curMesh.GetTextures().end()) curMesh.GetTextures().erase(it, curMesh.GetTextures().end());*/
    // 打印贴图路径
    /*print("贴图的数量: " << textures.size() << endl);
    for (int i = 0; i < textures.size(); ++i)
    {
        print("第" << i << "张贴图的路径" << textures[i] << endl);
    }*/
}


void Draw(Mesh& mesh, Shader& shader,
    glm::mat4 V, glm::mat4 P, glm::vec3 scaleSize = glm::vec3(1, 1, 1), glm::vec3 translate = glm::vec3(0, 0, 0),
    float angle = 0, glm::vec3 axis = glm::vec3(1, 0, 0))
{
    glm::mat4 M(1);
    M = glm::scale(M, scaleSize);
    M = glm::rotate(M, glm::radians(angle), axis);
    M = glm::translate(M, translate);

    shader.setMat4("model", M);
    shader.setMat4("view", V);
    shader.setMat4("projection", P);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.GetIndicies().size()), GL_UNSIGNED_INT, 0);
}

