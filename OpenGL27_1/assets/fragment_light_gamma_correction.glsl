#version 330 core
out vec4 FragColor;


uniform bool gamma = false;
uniform vec3 lightColor;

void main()
{
    
    vec3 color;
    if (gamma)
        color = pow(lightColor, vec3(1.0/2.2));
    else
        color = lightColor;
    FragColor = vec4(color, 1.0);
}