#version 460

in vec3 v_Pos;
out vec4 outColor;

void main()
{
    outColor = vec4(normalize(v_Pos) * 0.5 + 0.5, 1.0);
}
