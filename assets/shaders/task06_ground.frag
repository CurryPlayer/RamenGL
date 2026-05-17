#version 460

layout(location = 0) in vec3 vColor;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vFragPos;
layout(location = 3) in vec4 vFragPosLightSpace;
layout(location = 4) in vec2 vTexCoord;

layout(location = 0) out vec4 fragColor;

uniform sampler2D shadowMap;
uniform sampler2D groundTexture;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool useShadows;
uniform bool useTexture;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // check whether current frag pos is in shadow
    // add bias to reduce shadow acne
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(lightPos - vFragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    
    // keep shadow to 0.0 when outside the far_plane of light's frustum
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

void main()
{
    vec3 color = useTexture ? texture(groundTexture, vTexCoord).rgb : vec3(1.0);
    vec3 normal = normalize(vNormal);
    vec3 lightColor = vec3(1.0);
    
    // ambient
    vec3 ambient = 0.15 * lightColor;
    
    // diffuse
    vec3 lightDir = normalize(lightPos - vFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    vec3 viewDir = normalize(viewPos - vFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    
    // calculate shadow
    float shadow = useShadows ? ShadowCalculation(vFragPosLightSpace) : 0.0;                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    fragColor = vec4(lighting, 1.0);
}
