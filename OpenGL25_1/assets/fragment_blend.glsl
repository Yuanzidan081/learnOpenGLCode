#version 330 core

in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D texture1;
void main()
{
    
    // 1. discard�ķ���
    //vec4 texColor = texture(texture1, TexCoords);
    //if (texColor.a < 0.1)
    //    discard;
    //FragColor = texColor;

    // 2. blend�ķ���
    FragColor = texture(texture1, TexCoords);
}