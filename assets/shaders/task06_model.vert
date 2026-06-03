#version 460

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec3 a_Normal;

layout(location = 0) uniform mat4 u_ModelMat;
layout(location = 1) uniform mat4 u_ViewMat;
layout(location = 2) uniform mat4 u_ProjMat;
layout(location = 10) uniform mat4 u_LightSpaceMatrix;

out vec3 v_WorldPos;
out vec3 v_WorldNormal;
out vec4 v_FragPosLightSpace;

void main()
{
    v_WorldPos = vec3(u_ModelMat * vec4(a_Pos, 1.0f));
    v_WorldNormal = mat3(transpose(inverse(u_ModelMat))) * a_Normal;
    v_FragPosLightSpace = u_LightSpaceMatrix * vec4(v_WorldPos, 1.0f);
    gl_Position = u_ProjMat * u_ViewMat * vec4(v_WorldPos, 1.0f);
}
