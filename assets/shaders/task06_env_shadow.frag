#version 460

layout(location = 0) in vec3 vColor;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vFragPos;
layout(location = 3) in vec4 vFragPosLightSpace;

layout(location = 0) out vec4 fragColor;

uniform sampler2D shadowMap;
uniform samplerCube skybox;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool useShadows;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    if(projCoords.z > 1.0) shadow = 0.0;
    return shadow;
}

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 viewDir = normalize(viewPos - vFragPos);
    vec3 reflectDir = reflect(-viewDir, normal);
    vec3 envColor = texture(skybox, reflectDir).rgb;

    vec3 lightDir = normalize(lightPos - vFragPos);
    float shadow = useShadows ? ShadowCalculation(vFragPosLightSpace, normal, lightDir) : 0.0;
    
    vec3 ambient = 0.2 * envColor;
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * vec3(1.0);
    
    vec3 lighting = (ambient + (1.0 - shadow) * diffuse) * envColor;
    
    fragColor = vec4(lighting, 1.0);
}
