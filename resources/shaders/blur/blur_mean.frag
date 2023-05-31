#version 330 core

out vec4 o_colour;

in vec2 v_texCoord;

uniform sampler2D u_input;

uniform int u_kernelSize;
uniform float u_stdDev;
uniform float u_downscale;

vec3 sample_offset(int offsetX, int offsetY)
{
	vec2 texelSize = 1.0f / (textureSize(u_input, 0)/u_downscale);

	return texture(u_input, v_texCoord + texelSize * vec2(offsetX, offsetY)).rgb;
}

void main()
{
	vec3 accumulator = vec3(0.0f);

	int minX = int(floor(-u_kernelSize/2.0f));
	int maxX = int(ceil(u_kernelSize/2.0f));

	float weight = 1.0 / (u_kernelSize*u_kernelSize);

	for(int i = minX; i < maxX; i++)
	{
		for(int j = minX; j < maxX; j++)
		{
			accumulator += sample_offset(i, j) * weight;
		}
	}

	o_colour = vec4(accumulator, 1.0f);
}