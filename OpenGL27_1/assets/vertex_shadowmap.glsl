#version 330 core
layout (location = 0) in vec3 aPos;   // λ�ñ��� 
layout (location = 1) in vec3 aNormal; // ����λ��
layout (location = 2) in vec2 aTexture; // ��������



uniform mat4 lightSapceMatrix;
uniform mat4 model;
void main()
{
    gl_Position = lightSapceMatrix * model * vec4(aPos, 1.0);
}