#version 330 core
out vec4 FragColor;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
    mat3 TBN;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal;
uniform samplerCube depthMap;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;

uniform bool usePCF = false;
//uniform float bias = 0.005;
uniform bool blinn = true;
vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);
float ShadowCalculation(vec3 fragPos)
{
    //世界坐标片段和光源的方向向量，不需要使用归一化，生成的值是光源和其
    //最近的可见片段之间的归一化深度值，得到的是0-1，我们需要乘far_plane变为[0,far_palne]
    vec3 fragToLight = fragPos - lightPos;
    

    float currentDepth = length(fragToLight);
    float closestDepth = texture(depthMap, fragToLight).r;
    closestDepth *= far_plane;

    // 一般的shadow 
    //float bias = 0.05;
    //float shadowColor = currentDepth - bias > closestDepth  ? 0.0 : 1.0;
    
    //shadowColor = closestDepth / far_plane;
    
    // PCF
    float shadowColor = 0.0;
    if (usePCF)
    {
        // PCF -64 samples
        /*float bias = 0.05; 
        float samples = 4.0;
        float offset = 0.1;
        for(float x = -offset; x < offset; x += offset / (samples * 0.5))
        {
            for(float y = -offset; y < offset; y += offset / (samples * 0.5))
            {
                for(float z = -offset; z < offset; z += offset / (samples * 0.5))
                {
                    float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r; 
                    closestDepth *= far_plane;   // Undo mapping [0;1]
                    if(currentDepth - bias <= closestDepth)
                        shadowColor += 1.0;
                }
            }
        }
        shadowColor /= (samples * samples * samples);*/

        // PCF- reduce samples to 20
        float bias = 0.15;
        int samples = 20;
        float viewDistance = length(viewPos - fragPos);
        float diskRadius = 0.05;
        for(int i = 0; i < samples; ++i)
        {
            float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0; // adjust diskRadius by viewDistance
            float closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
            closestDepth *= far_plane;   // Undo mapping [0;1]
            if(currentDepth - bias <= closestDepth)
                shadowColor += 1.0;
        }
        shadowColor /= float(samples);
    }
    else
    {
        
        float bias = 0.05;
        shadowColor = currentDepth - bias > closestDepth  ? 0.0 : 1.0;
    }
   
        
    return shadowColor;

}
void main()
{
    vec3 color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    //vec3 normalMap = texture(texture_normal, fs_in.TexCoords).rgb;// [0, 1]
   vec3 normalMap = texture(texture_normal, fs_in.TexCoords).rgb * 2 - 1;// 映射到[-1, 1]
   normalMap = normalize(fs_in.TBN * normalMap);
    // ambient
    vec3 ambient = 0.05 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    //float diff = max(dot(lightDir, normal), 0.0);
    float diff = max(dot(normalMap, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);

    //计算阴影
    float shadow = ShadowCalculation(fs_in.FragPos);
    //shadow = min(shadow, 0.75); // reduce shadow strength a little: allow some diffuse/specular light in shadowed regions
    vec3 specular = vec3(0.3) * spec * color;
    
    vec3 result = ambient + shadow * (diffuse + specular);
    FragColor = vec4(result, 1.0);
}