#version 330 core
layout (location = 0) in vec3 aPos;   // 位置变量 
layout (location = 1) in vec2 aTexture; // 纹理坐标

out vec2 TexCoords;


void main()
{
    TexCoords = aTexture;
    gl_Position = vec4(aPos, 1.0);
}