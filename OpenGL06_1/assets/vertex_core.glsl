#version 330 core
layout (location = 0) in vec3 aPos;   // λ�ñ���������λ��ֵΪ 0 
layout (location = 1) in vec3 aColor; // ��ɫ����������λ��ֵΪ 1
layout (location = 2) in vec2 aTexture; // ��ɫ����������λ��ֵΪ 2

out vec3 ourColor; // ��Ƭ����ɫ�����һ����ɫ
out vec2 TexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 transform;
void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor; // ��ourColor����Ϊ���ǴӶ�����������õ���������ɫ
    TexCoord = aTexture;
}