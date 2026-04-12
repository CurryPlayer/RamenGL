#version 460

out vec4 outColor;

layout(location = 0) in vec3 in_Normal;
layout(location = 1) in vec3 in_ViewSpacePos;
layout(location = 2) in vec3 in_Color;

// Uniform for Debugging-Mode
layout(location = 99) uniform bool u_DebugNormals;

void main()
{
    if (u_DebugNormals) {
        // Normalenvektoren als Farben darstellen: von [-1,1] auf [0,1] mappen
        vec3 normalColor = (in_Normal + 1.0) * 0.5;
        outColor = vec4(normalColor, 1.0f);
    } else {
        outColor = vec4(in_Color, 1.0f);
    }
}
