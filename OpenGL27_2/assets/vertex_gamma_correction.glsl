#version 330 core
layout (location = 0) in vec3 aPos;   // 位置变量 
layout (location = 1) in vec3 aNormal; // 法线位置
layout (location = 2) in vec2 aTexture; // 纹理坐标

out VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vs_out.FragPos = vec3(model * vec4(aPos, 1));
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
    vs_out.TexCoords = aTexture;
}