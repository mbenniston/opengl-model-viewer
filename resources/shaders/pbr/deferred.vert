#version 330 core

layout(location=0) in vec3 a_position;
layout(location=1) in vec3 a_normal;
layout(location=2) in vec2 a_texcoord;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

out vec3 v_position;
out vec3 v_normal;
out vec2 v_texcoord;

void main()
{
    mat4 viewModel = u_view * u_model;
    vec4 viewSpacePostion = viewModel * vec4(a_position, 1.0);

    mat4 normalMatrix = transpose(inverse(viewModel));

    v_position = viewSpacePostion.xyz;
    v_normal = normalize((normalMatrix * vec4(a_normal, 0.0)).xyz);
    v_texcoord = a_texcoord;

    gl_Position = u_projection * viewSpacePostion;
}