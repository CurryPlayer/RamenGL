#version 460

// Since we only care about the depth buffer, we don't need to output any color.
// OpenGL will automatically write the gl_FragCoord.z value to the depth attachment
// of the currently bound framebuffer.

void main()
{
    // gl_FragDepth = gl_FragCoord.z; // Done automatically
}
