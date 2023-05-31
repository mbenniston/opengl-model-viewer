#version 330 core
layout (location = 0) in vec3 a_pos;

out vec3 v_cubePosition;

uniform mat4 u_mvp;

void main()
{
    v_cubePosition = a_pos;  
    vec4 position = u_mvp * vec4(a_pos, 1.0);
    gl_Position = position.xyww;
}