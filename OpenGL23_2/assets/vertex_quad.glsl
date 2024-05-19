#version 330 core
layout (location = 0) in vec2 aPos;   // 位置变量 
layout (location = 1) in vec2 aTexture; // 纹理坐标

out vec2 TexCoords;
void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    TexCoords = aTexture;
}