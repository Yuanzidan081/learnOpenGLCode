#version 330 core
layout (location = 0) in vec3 aPos;   // 位置变量 
layout (location = 1) in vec3 aNormal; // 法线位置
layout (location = 2) in vec2 aTexture; // 纹理坐标
layout (location = 3) in vec3 aTangent; // 切线位置




uniform mat4 model;
void main()
{
    gl_Position = model * vec4(aPos, 1.0);
}