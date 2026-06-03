#version 460

out vec4 outColor;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec4 v_FragPosLightSpace;

layout (location = 3) uniform vec3 u_LightPos;
layout (location = 4) uniform vec3 u_ViewPos;
layout (location = 99) uniform bool u_ShowTexture;
layout (location = 100) uniform bool u_UseShadows;

layout (binding = 1) uniform sampler2D u_ShadowMap;
layout (binding = 2) uniform sampler2D u_GroundTexture;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    if (!u_UseShadows) return 0.0;

    // 1. Perspective divide
    // Transform light-space position from homogeneous coordinates to NDC [-1, 1]
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // 2. Transform to [0, 1] range
    // Since depth textures and UVs are in the [0, 1] range, we must remap NDC.
    projCoords = projCoords * 0.5 + 0.5;
    
    // 3. Get closest depth value from light's perspective
    float closestDepth = texture(u_ShadowMap, projCoords.xy).r; 
    
    // 4. Get current depth of the fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // 5. Check whether current fragment is in shadow
    // We use a small bias to counteract "Shadow Acne"
    vec3 normal = normalize(v_Normal);
    vec3 lightDir = normalize(u_LightPos - v_FragPos);
    // Lower bias for better precision, but still preventing acne
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    
    // Keep shadows at 0.0 when outside the far_plane of light's frustum
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{
    vec3 color;
    if (u_ShowTexture) {
        // Use simple planar mapping (XZ coordinates)
        vec2 uv = v_FragPos.xz * 0.5;
        color = texture(u_GroundTexture, uv).rgb;
    } else {
        color = vec3(1.0); // Ground base color (white)
    }
    
    vec3 normal = normalize(v_Normal);
    vec3 lightColor = vec3(1.0);
    
    // Ambient
    vec3 ambient = 0.15 * lightColor;
    
    // Diffuse
    vec3 lightDir = normalize(u_LightPos - v_FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Calculate Shadow
    float shadow = ShadowCalculation(v_FragPosLightSpace);
    
    // Combine results (shadow only affects diffuse and specular, not ambient)
    vec3 lighting = (ambient + (1.0 - shadow) * diffuse) * color;
    
    outColor = vec4(lighting, 1.0);
}
