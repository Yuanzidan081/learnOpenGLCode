#include "Vertex.h"

void Mesh::SetLayout(const std::initializer_list<Layout>& layouts)
{
    layoutElements = layouts;
    int index = 0;
    int stride = 0;
    int offset = 0;
    for (auto& l : layoutElements)
    {
        l.Offset = offset;
        offset += l.AttriSize;
        stride += l.AttriSize;
    }
    for (auto& l : layoutElements)
    {
        l.Stride = stride;
        glEnableVertexAttribArray(index);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        SetVertexProperty(index, l.AttriSize, l.Stride, l.Offset);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        index++;
    }
}

void Mesh::SetInstanceLayout(const std::initializer_list<Layout>& layouts)
{
    layoutInstances = layouts;
    int index = layoutElements.size();
    int stride = 0;
    int offset = 0;
    for (auto& l : layoutInstances)
    {
        l.Offset = offset;
        offset += l.AttriSize;
        stride += l.AttriSize;
    }
    for (auto& l : layoutInstances)
    {
        l.Stride = stride;
        glEnableVertexAttribArray(index);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        SetVertexProperty(index, l.AttriSize, l.Stride, l.Offset);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribDivisor(index, 1);
        index++;
    }
    
    //glBindVertexArray(0);
}
