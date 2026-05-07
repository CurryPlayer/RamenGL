#version 460

in vec3 v_Pos;
out vec4 outColor;

layout(location = 99) uniform bool u_showTexture;

layout(binding = 0) uniform samplerCube u_Cubemap;

void main()
{
    if (u_showTexture) {
        vec4 texColor = texture(u_Cubemap, v_Pos);
        outColor = texColor;
    } else {
        outColor = vec4(normalize(v_Pos) * 0.5 + 0.5, 1.0);
    }

}
