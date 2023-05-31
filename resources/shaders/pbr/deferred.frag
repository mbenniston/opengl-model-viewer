#version 330 core

layout(location=0) out vec3 o_albedo;
layout(location=1) out vec3 o_geometry;
layout(location=2) out vec3 o_normal;
layout(location=3) out vec3 o_roughMetal;
layout(location=4) out vec3 o_emissive;

in vec3 v_position;
in vec3 v_normal;
in vec2 v_texcoord;

uniform sampler2D u_base;
uniform sampler2D u_roughnessMetallic;
uniform sampler2D u_emissive;
uniform sampler2D u_occlusion;

uniform vec4 u_baseFactor;
uniform float u_metalFactor;
uniform float u_roughFactor;
uniform vec3 u_emissiveFactor;

void main()
{
    vec4 roughnessMetallic_color = texture(u_roughnessMetallic, v_texcoord);
    float roughness = roughnessMetallic_color.g * u_roughFactor;
    float metallic = roughnessMetallic_color.b * u_metalFactor;
    float occlusion = roughnessMetallic_color.r;

    vec4 base_color = pow(texture(u_base, v_texcoord), vec4(2.2)) * u_baseFactor;
    vec3 emissive =  pow(texture(u_emissive, v_texcoord).rgb, vec3(2.2));
	
    o_albedo = base_color.rgb;
    o_geometry = v_position;
    o_normal = normalize(v_normal);
    o_roughMetal = vec3(occlusion, roughness, metallic);
    o_emissive = emissive * u_emissiveFactor;
}