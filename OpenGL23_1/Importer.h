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
//EBO�������ԣ���glDrawElements�ĵڶ����͵��ĸ��������õ�
struct IndicesElements
{
    unsigned int IndOffset;//����ƫ��
    unsigned int IndNum;// ������
    unsigned int materialId; // ���ʵ�ID��
};
std::vector<IndicesElements> indicesElement;//EBO��������


struct Material
{
    std::string diff;
    std::string spec;
    std::string refl;
};

std::vector<Material> materials; // ���ʵ�����
std::vector<std::string> textures;//��ͼ��·��

int GetTextureID(unordered_map<string, int>& textureMap, string& textureStr)
{
    if (textureMap.find(textureStr) != textureMap.end())
        return textureMap[textureStr];
    return -1;
}

Mesh ImportModel(const std::string& path)
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

        // print("����������: " << mesh->mNumFaces * 3 << endl);
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
    vector<aiTextureType> textureType = { aiTextureType_DIFFUSE , aiTextureType_SPECULAR ,aiTextureType_AMBIENT };
    vector<string> textureStr = { "diffuse: ", "specular: ", "reflect: " };
    string* p;
    for (int i = 0; i < numMaterials; ++i)
    {
        // print("��" << i << "������" << endl);
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
    // ��ӡ��ͼ·��
    /*print("��ͼ������: " << textures.size() << endl);
    for (int i = 0; i < textures.size(); ++i)
    {
        print("��" << i << "����ͼ��·��" << textures[i] << endl);
    }*/
    return Mesh(modelVertices, modelIndices);



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

void Draw(IndicesElements& IE, Shader& shader,
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
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(IE.IndNum), GL_UNSIGNED_INT, (void*)(IE.IndOffset * sizeof(float)));
}