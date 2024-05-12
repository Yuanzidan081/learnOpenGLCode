#version 330 core
layout (location = 0) in vec3 aPos;   // λ�ñ��� 
layout (location = 1) in vec3 aColor;  // ��ɫ����

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

