#version 460

in vec3 v_WorldPos;
in vec3 v_WorldNormal;
out vec4 outColor;

layout(location = 3) uniform vec3 u_CamPos;
layout(binding = 0) uniform samplerCube u_Cubemap;

void main()
{
    vec3 I = normalize(v_WorldPos - u_CamPos);
    vec3 N = normalize(v_WorldNormal);
    vec3 R = reflect(I, N);
    
    outColor = vec4(texture(u_Cubemap, R).rgb, 1.0);
}
