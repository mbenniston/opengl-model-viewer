#version 330 core

layout(location = 0) out vec4 o_albedo;
layout(location = 1) out vec3 o_geometry;
layout(location = 2) out vec3 o_normal;
layout(location = 3) out vec4 o_temperature;

in vec3 v_position;
in vec3 v_normal;
in vec2 v_texcoord;

uniform sampler2D u_base;
uniform vec4 u_baseFactor;

uniform float u_blue, u_yellow, u_alpha, u_beta;

void main()
{
    vec4 base_color = pow(texture(u_base, v_texcoord), vec4(2.2f)) * u_baseFactor;

	o_albedo = base_color;
    o_geometry = v_position;
    o_normal = normalize(v_normal);
    o_temperature = vec4(u_blue, u_yellow, u_alpha, u_beta);
}