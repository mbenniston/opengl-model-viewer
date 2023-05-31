#version 330 core

layout(location=0) out vec3 o_albedo;
layout(location=1) out vec3 o_geometry;
layout(location=2) out vec3 o_normal;
layout(location=3) out vec3 o_roughMetal;
layout(location=4) out vec3 o_emissive;

in vec3 v_position;
in vec3 v_normal;

void main()
{
    o_albedo = (normalize(v_normal) + vec3(1.0f)) * 0.5f;
    o_geometry = v_position;
    o_normal = normalize(v_normal);
    o_roughMetal = vec3(1.0f, 1.0f, 0.0f);
    o_emissive = o_albedo.rgb * 10.0f;
}