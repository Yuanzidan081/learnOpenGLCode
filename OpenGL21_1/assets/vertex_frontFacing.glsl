#version 330 core
layout (location = 0) in vec3 aPos;   // λ�ñ��� 
layout (location = 1) in vec3 aNormal;  // ��ɫ����
layout (location = 2) in vec2 aTexCoord; // ��������

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;
void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoords = aTexCoord;
}