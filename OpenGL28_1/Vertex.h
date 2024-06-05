#pragma once
#include <glad/glad.h>
#include "Layout.h"
#include <vector>
#include <initializer_list>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <unordered_map>
#include "Texture.h"
#include "Shader.h"
#include <algorithm>
int GetTextureID(std::unordered_map<std::string, int>& textureMap, std::string& textureStr);


struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
};
struct IndicesElements
{
    unsigned int IndOffset;//顶点偏移
    unsigned int IndNum;// 顶点数
    unsigned int materialId; // 材质的ID号
};
struct Material
{
    std::string diff;
    std::string spec;
    std::string refl;
};
class Mesh
{
public:
    Mesh() {}
    Mesh(float* vertices, int verSize, unsigned int* indices, int indSize)
    {
        //3.产生Buffer和VertexArray
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);//glGenBuffers和缓冲ID生成一个VBO对象
        glGenBuffers(1, &EBO);

        //4.绑定Buffer和VertexArray
        glBindVertexArray(VAO);//绑定VAO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, verSize, vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indSize, indices, GL_STATIC_DRAW);
    }

    Mesh(float* vertices, int verSize, bool useInstance = false)
    {
        //3.产生Buffer和VertexArray
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);//glGenBuffers和缓冲ID生成一个VBO对象
        if (useInstance)
        {
            glGenBuffers(1, &instanceVBO);
        }
        //4.绑定Buffer和VertexArray
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, verSize, vertices, GL_STATIC_DRAW);
        glBindVertexArray(VAO);//绑定VAO

        
    }

    void setInstanceVec2(glm::vec2* instance, int instanceSize)
    {

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, instanceSize, instance, GL_STATIC_DRAW);

    }

    void setInstanceMat4(glm::mat4* instance, int instanceSize)
    {
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, instanceSize, instance, GL_STATIC_DRAW);
        //glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, bool useInstance = false)
    {
        //3.产生Buffer和VertexArray
        this->vertices = vertices;
        this->indices = indices;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);//glGenBuffers和缓冲ID生成一个VBO对象
        glGenBuffers(1, &EBO);
        if (useInstance)
        {
            glGenBuffers(1, &instanceVBO);
        }
        //4.绑定Buffer和VertexArray
        glBindVertexArray(VAO);//绑定VAO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    }

    ~Mesh()
    {
        UnBindVertex();
        DeleteVertex();
    }
    //5.创建指针
    // 位置属性    
    void SetVertexProperty(int index, int attriSize, int stride, int offset, int unitSize)
    {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, attriSize, GL_FLOAT, GL_FALSE, 
            stride * unitSize, (void*)(offset * unitSize));
    }
    void BindVertex()
    {
        glBindVertexArray(VAO);
    }
    void UnBindVertex()
    {
        glBindVertexArray(0);
    }
    void DeleteVertex()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
    // 贴图的上一级路径
    void SetModelTextureMap(const std::string& path)
    {
        
        for (int j = 0; j < textures.size(); ++j)
        {
            modelTexture.push_back(Texture((path + textures[j]).c_str(), j, true));
            textureMap.insert({ textures[j], j });
            std::cout << textures[j] << " " << j << std::endl;
        }
    }
    void DrawModel(Shader& shader,
        glm::mat4& V, glm::mat4& P, glm::vec3 scaleSize = glm::vec3(1, 1, 1), glm::vec3 translate = glm::vec3(0, 0, 0),
        float angle = 0, glm::vec3 axis = glm::vec3(1, 0, 0))
    {
        for (int i = 0; i < indicesElement.size(); ++i)
        {
            int matID = indicesElement[i].materialId;
            int difftextureID = GetTextureID(textureMap, materials[matID].diff);
            int spectextureID = GetTextureID(textureMap, materials[matID].diff);
            modelTexture[difftextureID].Activate();
            shader.setInt("texture_diffuse1", difftextureID);
            DrawUnit(indicesElement[i], shader, V, P, scaleSize ,
                translate, angle, axis);
        }
    }

    void DrawModel(Shader& shader, glm::mat4 M,
        glm::mat4 V, glm::mat4 P)
    {
        for (int i = 0; i < indicesElement.size(); ++i)
        {
            int matID = indicesElement[i].materialId;
            int difftextureID = GetTextureID(textureMap, materials[matID].diff);
            int spectextureID = GetTextureID(textureMap, materials[matID].diff);
            modelTexture[difftextureID].Activate();
            shader.setInt("texture_diffuse1", difftextureID);
            DrawUnit(indicesElement[i], shader, M, V, P);
        }
    }

    // 用在shadowMap上
    void DrawModel(Shader& shader, glm::mat4 M)
    {
        for (int i = 0; i < indicesElement.size(); ++i)
        {
            
            DrawUnit(indicesElement[i], shader, M);
        }
    }
    

    void DrawModelInstance(Shader& shader,
        glm::mat4& V, glm::mat4& P, int amount)
    {
        for (int i = 0; i < indicesElement.size(); ++i)
        {
            int matID = indicesElement[i].materialId;
            int difftextureID = GetTextureID(textureMap, materials[matID].diff);
            int spectextureID = GetTextureID(textureMap, materials[matID].diff);
            modelTexture[difftextureID].Activate();
            shader.setInt("texture_diffuse1", difftextureID);
            DrawUnitInstance(indicesElement[i], shader, V, P, amount);
        }
    }

    void DrawUnit(IndicesElements& IE, Shader& shader,
        glm::mat4& V, glm::mat4& P, glm::vec3 scaleSize = glm::vec3(1, 1, 1), glm::vec3 translate = glm::vec3(0, 0, 0),
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

    void DrawUnitInstance(IndicesElements& IE, Shader& shader,
        glm::mat4& V, glm::mat4& P, int amount)
    {
        shader.setMat4("view", V);
        shader.setMat4("projection", P);
        glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(IE.IndNum), GL_UNSIGNED_INT, (void*)(IE.IndOffset * sizeof(float)), amount);
    }

    void DrawUnit(IndicesElements& IE, Shader& shader, glm::mat4& M,
        glm::mat4& V, glm::mat4& P)
    {
        shader.setMat4("model", M);
        shader.setMat4("view", V);
        shader.setMat4("projection", P);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(IE.IndNum), GL_UNSIGNED_INT, (void*)(IE.IndOffset * sizeof(float)));
    }

    void DrawUnit(IndicesElements& IE, Shader& shader, glm::mat4& M
        )
    {
        shader.setMat4("model", M);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(IE.IndNum), GL_UNSIGNED_INT, (void*)(IE.IndOffset * sizeof(float)));
    }
    void SetUniqueTexture()
    {
        std::vector<std::string>::iterator it;
        std::sort(textures.begin(), textures.end());
        it = std::unique(textures.begin(), textures.end());
        if (it != textures.end()) textures.erase(it, textures.end());
    }

    void SetLayout(const std::initializer_list<Layout>& layouts);
    void SetInstanceLayout(const std::initializer_list<Layout>& layouts);
    const std::vector<Vertex>& GetVertices() const { return vertices; }
    const std::vector<unsigned int>& GetIndicies() const { return indices; }

    unsigned int& GetVAO() { return VAO; }

    std::vector<IndicesElements>& GetIndicesElements() { return indicesElement; }
    std::vector<Material>& GetMaterial() { return materials; }
    std::vector<std::string>& GetTextures() { return textures; }
private:
    unsigned int VAO, VBO, EBO;
    unsigned int instanceVBO;
    std::vector<Layout> layoutElements;
    std::vector<Layout> layoutInstances;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Assimp模型生成的相关的属性
    std::vector<IndicesElements> indicesElement;//EBO属性数组
    std::vector<Material> materials; // 材质的容器
    std::vector<std::string> textures;//贴图的路径
    std::unordered_map<std::string, int> textureMap; // 贴图路径和贴图槽编号的哈希表
    std::vector<Texture> modelTexture;
    
};