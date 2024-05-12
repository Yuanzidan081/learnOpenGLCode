#version 330 core
layout (location = 0) in vec3 aPos;   // 位置变量 
layout (location = 1) in vec3 aColor;  // 颜色向量
layout (location = 2) in vec2 aTexCoord; // 纹理坐标

layout (std140) uniform Matrices
{
    uniform mat4 projection;
    uniform mat4 view;
};

uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

