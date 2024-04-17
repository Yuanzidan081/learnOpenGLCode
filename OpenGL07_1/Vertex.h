#pragma once
#include <glad/glad.h>
#include "Layout.h"
#include <vector>
#include <initializer_list>
class Vertex
{
public:
    Vertex(float* vertices, int verSize, unsigned int* indices, int indSize)
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

    Vertex(float* vertices, int verSize)
    {
        //3.����Buffer��VertexArray
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);//glGenBuffers�ͻ���ID����һ��VBO����

        //4.��Buffer��VertexArray
        glBindVertexArray(VAO);//��VAO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, verSize, vertices, GL_STATIC_DRAW);
    }
        
    //5.����ָ��
    // λ������    
    void SetVertexProperty(int index, int attriSize, int stride, int offset)
    {
        glVertexAttribPointer(index, attriSize, GL_FLOAT, GL_FALSE, 
            stride * sizeof(float), (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(index);
    }
    void BindVertex()
    {
        glBindVertexArray(VAO);
    }
    void DeleteVertex()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void SetLayout(const std::initializer_list<Layout>& layouts);

private:
    unsigned int VAO, VBO, EBO;
    std::vector<Layout> layoutElements;
    
};