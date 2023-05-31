#version 330 core
layout(location=0) out vec3 o_color;

in vec3 v_cubePosition;

uniform samplerCube u_skyboxSampler;

void main()
{		
    vec3 color = texture(u_skyboxSampler, v_cubePosition).rgb;
	color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 

	o_color = color;
}