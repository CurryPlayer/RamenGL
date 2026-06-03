#version 460

layout (location = 0) in vec3 in_position;

// The Light Space Matrix combines the light's view and projection matrices
layout (location = 0) uniform mat4 u_LightSpaceMatrix;
layout (location = 1) uniform mat4 u_ModelMat;

void main()
{
    // Transform vertex position into light space
    // This is similar to standard MVP transformation, but from the light's perspective
    gl_Position = u_LightSpaceMatrix * u_ModelMat * vec4(in_position, 1.0);
}
