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
        SetVertexProperty(index, l.AttriSize, l.Stride, l.Offset, l.UnitSize);
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
        offset += 1;
        stride += 1;
    }
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    for (auto& l : layoutInstances)
    {
        l.Stride = stride;
        glEnableVertexAttribArray(index);
        
        SetVertexProperty(index, l.AttriSize, l.Stride, l.Offset, l.UnitSize);
        
        glVertexAttribDivisor(index, 1);
        index++;
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindVertexArray(0);
}
int GetTextureID(std::unordered_map<std::string, int>& textureMap, std::string& textureStr)
{
    if (textureMap.find(textureStr) != textureMap.end())
        return textureMap[textureStr];
    return -1;
}