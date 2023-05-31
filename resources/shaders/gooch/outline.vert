#version 330 core

layout(location=0) in vec3 a_position;
layout(location=1) in vec3 a_normal;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

void main()
{
    vec3 clipSpaceNormal = normalize(mat3(u_projection * u_view * u_model) * a_normal);
    vec4 clipSpacePosition = u_projection * u_view * u_model * vec4(a_position, 1.0f);

    clipSpacePosition.xy += normalize(clipSpaceNormal.xy) / vec2(1920.0, 1080.0) * 10.0f * clipSpacePosition.w * 2;

    gl_Position = clipSpacePosition;
}