#version 330 core

layout(location=0) in vec3 a_position;
layout(location=1) in vec3 a_normal;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

out VS_OUT {
    vec3 position;
    vec3 normal;
} vs_out;

void main()
{
    mat4 viewModel = u_view * u_model;
    vec4 viewSpacePostion = viewModel * vec4(a_position, 1.0f);

    mat4 normalMatrix = transpose(inverse(viewModel));

    vs_out.position = viewSpacePostion.xyz;
    vs_out.normal = normalize((normalMatrix * vec4(a_normal, 0.0f)).xyz);

    gl_Position = u_projection * viewSpacePostion;
}