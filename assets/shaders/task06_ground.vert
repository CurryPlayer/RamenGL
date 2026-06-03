#version 460

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_color;

layout (location = 0) uniform mat4 u_ModelMat;
layout (location = 1) uniform mat4 u_ViewMat;
layout (location = 2) uniform mat4 u_ProjMat;
layout (location = 10) uniform mat4 u_LightSpaceMatrix;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec4 v_FragPosLightSpace;

void main()
{
    v_FragPos = vec3(u_ModelMat * vec4(in_position, 1.0));
    // Transform normal to world space (assuming no non-uniform scale)
    v_Normal = mat3(transpose(inverse(u_ModelMat))) * in_normal;
    
    // Transform vertex position into light space for shadow mapping
    v_FragPosLightSpace = u_LightSpaceMatrix * vec4(v_FragPos, 1.0);
    
    gl_Position = u_ProjMat * u_ViewMat * vec4(v_FragPos, 1.0);
}
