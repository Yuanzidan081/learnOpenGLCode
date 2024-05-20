#version 330 core
layout (location = 0) in vec3 aPos;   // 位置变量 
layout (location = 1) in vec2 aNormal; // 纹理坐标
layout (location = 2) in vec2 aTexture; // 纹理坐标
layout (location = 3) in mat4 instanceMatrix; // 实例化的矩阵

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
void main()
{
    gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0);
    TexCoords = aTexture;
}