#version 330 core
in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;
out vec4 FragColor;


uniform vec3 lightColor;
uniform vec3 objectColor;
uniform int specFactor;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    

    float speculateStrength = 0.5;
    vec3 viewDir = normalize(FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specFactor);
    vec3 specular = speculateStrength * spec * lightColor;
    vec3 result = (ambient + diffuse + specular) * objectColor;

    
    FragColor = vec4(result, 1.0);
}