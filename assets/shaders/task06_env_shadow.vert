#version 460

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;

layout(location = 0) out vec3 vColor;
layout(location = 1) out vec3 vNormal;
layout(location = 2) out vec3 vFragPos;
layout(location = 3) out vec4 vFragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 lightSpaceMatrix;

void main()
{
    vFragPos = vec3(model * vec4(aPos, 1.0));
    vNormal = mat3(transpose(inverse(model))) * aNormal;
    vColor = aColor;
    vFragPosLightSpace = lightSpaceMatrix * vec4(vFragPos, 1.0);
    gl_Position = proj * view * vec4(vFragPos, 1.0);
}
