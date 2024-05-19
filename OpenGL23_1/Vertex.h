#pragma once
#include <glad/glad.h>
#include "Layout.h"
#include <vector>
#include <initializer_list>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh
{
public:
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
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }

    Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
    {
        //3.产生Buffer和VertexArray
        this->vertices = vertices;
        this->indices = indices;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);//glGenBuffers和缓冲ID生成一个VBO对象
        glGenBuffers(1, &EBO);
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
    void SetVertexProperty(int index, int attriSize, int stride, int offset)
    {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, attriSize, GL_FLOAT, GL_FALSE, 
            stride * sizeof(float), (void*)(offset * sizeof(float)));
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

    void SetLayout(const std::initializer_list<Layout>& layouts);
    void SetInstanceLayout(const std::initializer_list<Layout>& layouts);
    const std::vector<Vertex>& GetVertices() const { return vertices; }
    const std::vector<unsigned int>& GetIndicies() const { return indices; }
private:
    unsigned int VAO, VBO, EBO;
    unsigned int instanceVBO;
    std::vector<Layout> layoutElements;
    std::vector<Layout> layoutInstances;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
};