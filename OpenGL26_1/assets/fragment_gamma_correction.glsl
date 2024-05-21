#version 330 core
out vec4 FragColor;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D floorTexture;

uniform vec3 viewPos;

uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform bool gamma = false;

vec3 BlinnPhong(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor)
{
    // diffuse
    vec3 lightDir = normalize(lightPos - fragPos);
    
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightPos, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfwayDir, normal), 0.0), 32);
    vec3 specular = spec * lightColor;
    // attenuation
    float max_distance = 1.5;
    float distance = length(lightPos - fragPos);
    float atternuation = 1.0 / (gamma ? distance * distance  : distance);

    diffuse *= atternuation;
    specular *= atternuation;
    return diffuse + specular;
}
void main()
{
    vec3 normal = normalize(fs_in.Normal);
    vec3 color = texture(floorTexture, fs_in.TexCoords).rgb;
    // ambient
    vec3 ambient = 0.05 * color;
    vec3 lighting = vec3(0.0);
    for (int i = 0; i < 4; ++i)
    {
        lighting += BlinnPhong(normal, fs_in.FragPos, lightPositions[i], lightColors[i]);
    }
    color = ambient + lighting * color;
    if (gamma)
        color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}