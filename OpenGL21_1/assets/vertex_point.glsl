#version 330 core
layout (location = 0) in vec3 aPos;   // 位置变量 
layout (location = 1) in vec3 aColor;  // 颜色向量

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Color;
void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    gl_PointSize =  gl_Position.z * 2;  
    Color = aColor;
}

