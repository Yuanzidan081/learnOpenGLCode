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
        //3.����Buffer��VertexArray
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);//glGenBuffers�ͻ���ID����һ��VBO����
        glGenBuffers(1, &EBO);

        //4.��Buffer��VertexArray
        glBindVertexArray(VAO);//��VAO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, verSize, vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indSize, indices, GL_STATIC_DRAW);
    }

    Mesh(float* vertices, int verSize)
    {
        //3.����Buffer��VertexArray
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);//glGenBuffers�ͻ���ID����һ��VBO����

        //4.��Buffer��VertexArray
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, verSize, vertices, GL_STATIC_DRAW);
        glBindVertexArray(VAO);//��VAO
    }

    Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
    {
        //3.����Buffer��VertexArray
        this->vertices = vertices;
        this->indices = indices;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);//glGenBuffers�ͻ���ID����һ��VBO����
        glGenBuffers(1, &EBO);
        //4.��Buffer��VertexArray
        glBindVertexArray(VAO);//��VAO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    }
        
    //5.����ָ��
    // λ������    
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

    const std::vector<Vertex>& GetVertices() const { return vertices; }
    const std::vector<unsigned int>& GetIndicies() const { return indices; }
private:
    unsigned int VAO, VBO, EBO;
    std::vector<Layout> layoutElements;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
};