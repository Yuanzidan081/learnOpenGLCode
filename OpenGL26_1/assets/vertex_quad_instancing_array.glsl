#version 330 core
layout (location = 0) in vec2 aPos;   // 位置变量 
layout (location = 1) in vec3 aColor; // 纹理坐标
layout (location = 2) in vec2 aOffset; // 偏移
out vec3 fColor;


void main()
{
    gl_Position = vec4(aPos + aOffset, 0.0, 1.0);
    fColor = aColor;
}