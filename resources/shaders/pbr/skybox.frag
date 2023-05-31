#version 330 core
layout(location=0) out vec3 o_color;
layout(location=1) out vec3 o_geometry;
layout(location=2) out vec3 o_normal;
layout(location=3) out vec3 o_roughMetal;
layout(location=4) out vec3 o_emissive;

in vec3 v_cubePosition;

uniform samplerCube u_skyboxSampler;

void main()
{		
    vec3 color = texture(u_skyboxSampler, v_cubePosition).rgb;
    
    o_color = color;
    o_geometry = vec3(0.0f);
    o_normal = vec3(0.0f);
    o_roughMetal = vec3(0.0f);
    o_emissive = vec3(0.0f);
}