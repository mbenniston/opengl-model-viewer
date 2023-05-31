#version 330 core

out vec4 o_colour;

in vec2 v_texCoord;

uniform sampler2D u_input;

uniform int u_kernelSize;
uniform float u_stdDev;
uniform float u_downscale;

vec3 sample_offset(int offsetX, int offsetY)
{
	vec2 texelSize = 1.0f / (textureSize(u_input, 0)/max(u_downscale, 0.0001f));

	return texture(u_input, v_texCoord + texelSize * vec2(offsetX, offsetY)).rgb;
}

const float PI = 3.141592653589793;

float guass1D(int offset, float stddev)
{
	return (1.0f / (stddev * sqrt(2 * PI))) * exp(-0.5f * (pow(offset - 0,2.0f)/pow(stddev,2.0f))); 
}

float guass2D(int offsetX, int offsetY, float stddev)
{
	return guass1D(offsetX, stddev) * guass1D(offsetY, stddev);
}

void main()
{
	vec3 accumulator = vec3(0.0f);

	int minX = int(floor(-u_kernelSize/2.0f));
	int maxX = int(ceil(u_kernelSize/2.0f));

	for(int i = minX; i < maxX; i++)
	{
		for(int j = minX; j < maxX; j++)
		{
			float weight = guass2D(i, j, u_stdDev);
			accumulator += sample_offset(i, j) * weight;
		}
	}

	o_colour = vec4(accumulator, 1.0f);
}