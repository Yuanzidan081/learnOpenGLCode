#version 330 core
out vec4 FragColor;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D shadowMap;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool usePCF = false;
//uniform float bias = 0.005;
uniform bool blinn = true;
float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;

    // 检查当前片段是否在阴影中
    //float bias = max(0.05 * (1.0 - dot(fs_in.Normal, normalize(lightPos - fs_in.FragPos))), 0.005);
    
    // 一般的shadow 
    float bias = 0.0005;
    //float shadowColor = currentDepth - bias > closestDepth  ? 0.0 : 1.0;
    
    // PCF
    float shadowColor = 0.0;
    if (usePCF)
    {
        
        vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
        for (int x = -1; x <= 1; ++x)
        {
            for (int y = -1; y <= 1; ++y)
            {
                float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                shadowColor += currentDepth - bias > pcfDepth ? 0.0 : 1.0;
            }
        }
        shadowColor /= 9.0;
    }
    else
    {
        shadowColor = currentDepth - bias > closestDepth  ? 0.0 : 1.0;
    }
    
    if(projCoords.z > 1.0)
        shadowColor = 1.0;
    return shadowColor;

}
void main()
{
    vec3 color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    // ambient
    vec3 ambient = 0.05 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightPos, normal);
    float spec = 0.0;
    if (blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(halfwayDir, normal), 0.0), 32);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
    }
    //计算阴影
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    shadow = min(shadow, 0.75); // reduce shadow strength a little: allow some diffuse/specular light in shadowed regions
    vec3 specular = vec3(0.3) * spec * color;

    vec3 result = ambient + shadow * (diffuse + specular);
    FragColor = vec4(result, 1.0);
}