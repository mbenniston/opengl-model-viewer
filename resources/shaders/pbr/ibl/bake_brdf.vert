#version 330 core

layout(location=0) in vec2 a_position;

out vec2 v_texCoord;

void main()
{
    v_texCoord = (a_position + vec2(1.0, 1.0)) * 0.5;

    gl_Position = vec4(a_position, 0.0f, 1.0);
}