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
        SetVertexProperty(index, l.AttriSize, l.Stride, l.Offset);
        index++;
    }
    glBindVertexArray(0);
}
