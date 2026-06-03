#version 460

in vec3 v_WorldPos;
in vec3 v_WorldNormal;
in vec4 v_FragPosLightSpace;
out vec4 outColor;

layout(location = 3) uniform vec3 u_CamPos;
layout(location = 11) uniform vec3 u_LightPos;
layout(binding = 0) uniform samplerCube u_Cubemap;
layout(binding = 1) uniform sampler2D u_ShadowMap;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // Projection division
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Remap to [0,1]
    projCoords = projCoords * 0.5 + 0.5;
    
    float closestDepth = texture(u_ShadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    
    // Bias to prevent shadow acne
    vec3 normal = normalize(v_WorldNormal);
    vec3 lightDir = normalize(u_LightPos - v_WorldPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    
    // Keep area outside light frustum unshadowed
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{
    vec3 I = normalize(v_WorldPos - u_CamPos);
    vec3 N = normalize(v_WorldNormal);
    vec3 R = reflect(I, N);
    
    vec3 reflectColor = texture(u_Cubemap, R).rgb;
    
    // Ambient
    vec3 ambient = 0.2 * reflectColor;
    
    // Diffuse-like contribution (using reflection as base)
    float shadow = ShadowCalculation(v_FragPosLightSpace);
    
    // Combine: Shadow affects most of the reflection but leaves some ambient
    vec3 finalColor = (ambient + (1.0 - shadow) * (reflectColor * 0.8));
    
    outColor = vec4(finalColor, 1.0f);
}
