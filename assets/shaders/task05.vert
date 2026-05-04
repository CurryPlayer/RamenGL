#version 460


/* NOTE:
   One could use 'glGetUniformLocation' on CPU-side instead
   of fixed location = ... qualifiers.
   But this is not recommended anymore.
   @See: OpenGL Superbible 7, page 156.
*/
layout(location = 0) in vec3 a_Pos;

layout(location = 0) uniform mat4 u_ModelMat;
layout(location = 1) uniform mat4 u_ViewMat;
layout(location = 2) uniform mat4 u_ProjMat;

out vec3 v_Pos;

void main()
{
    v_Pos = a_Pos;
    gl_Position = u_ProjMat * u_ViewMat * u_ModelMat * vec4(a_Pos, 1.0);
}
