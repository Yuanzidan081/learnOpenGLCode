#version 330 core
layout (location = 0) in vec3 aPos;   // 位置变量 
layout (location = 1) in vec3 aNormal;  // 颜色向量
layout (location = 2) in vec2 aTexCoord; // 纹理坐标

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;
void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoords = aTexCoord;
}