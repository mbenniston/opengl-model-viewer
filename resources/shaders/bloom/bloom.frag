#version 330 core

out vec4 o_colour;

in vec2 v_texCoord;

uniform sampler2D u_source;
uniform sampler2D u_bloom;
uniform sampler2D u_lensDirt;

uniform float u_bloomFactor;
uniform float u_lensDirtIntensity;

void main()
{
	vec3 source = texture(u_source, v_texCoord).rgb;
	vec3 bloom = texture(u_bloom, v_texCoord).rgb;
	vec3 lensDirt = texture(u_lensDirt, v_texCoord).rgb;

	vec3 color = mix(source, bloom + bloom * lensDirt * u_lensDirtIntensity, u_bloomFactor);

	// tone map
	color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 

	o_colour = vec4(color, 1.0f);
}