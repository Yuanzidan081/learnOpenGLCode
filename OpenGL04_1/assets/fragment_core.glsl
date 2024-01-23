#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixFactor;
void main()
{
    vec4 t1 = vec4(ourColor, 1.0);
    vec4 t2 = texture(texture1, TexCoord); //采样器，纹理坐标
    vec4 t3 = texture(texture2, vec2(1-TexCoord.x, TexCoord.y));
    FragColor = mix(t2, t3, mixFactor);
}