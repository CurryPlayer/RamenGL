#version 460

out vec4 outColor;

layout(location = 0) in vec3 in_Normal;
layout(location = 1) in vec3 in_ViewSpacePos;
layout(location = 2) in vec3 in_Color;

// Uniform for Debugging-Mode
layout(location = 99) uniform bool u_DebugNormals;

// Lighting uniforms
layout(location = 4) uniform vec3 u_LightPosition; // Light position in view space
layout(location = 5) uniform vec3 u_LightColor;    // Light color
layout(location = 6) uniform vec3 u_AmbientColor;  // Ambient light color

void main()
{
    if (u_DebugNormals) {
        // represent normals as colors: map from [-1,1] to [0,1]
        vec3 normalColor = (in_Normal + 1.0) * 0.5;
        outColor = vec4(normalColor, 1.0f);
    } else {
        // -> without lighting, just output the vertex color
        //outColor = vec4(in_Color, 1.0f);

        // -> with lighting, implement Lambert's cosine law for diffuse shading
        // Lambert's cosine law: diffuse = max(0, dot(N, L)) * lightColor * materialColor
        // see https://moodle.hm.edu/pluginfile.php/2052333/mod_resource/content/2/Spotlights%20und%20Abschw%C3%A4chungsfaktoren.html
        vec3 N = normalize(in_Normal);
        vec3 L = normalize(u_LightPosition - in_ViewSpacePos); // Light direction / light direction
        float diffuse = max(0.0, dot(N, L));
        vec3 diffuseColor = diffuse * u_LightColor * in_Color;

        // Add ambient light
        vec3 finalColor = diffuseColor + u_AmbientColor * in_Color;
        //vec3 finalColor = diffuseColor;

        outColor = vec4(finalColor, 1.0f);
    }
}
