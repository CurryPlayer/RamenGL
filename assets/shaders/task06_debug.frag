#version 460

in vec3 v_Color;
out vec4 outColor;

// simple shader to visualize the normals as colors
void main()
{
    outColor = vec4(v_Color, 1.0f);
}
