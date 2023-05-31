#version 330 core

out float o_ambientOcculusion;

in vec2 v_texCoord;

uniform mat4 u_projection;
uniform float u_distance;
uniform float u_bias;
uniform int u_samples;
uniform vec2 u_noiseScale;

uniform sampler2D u_geometry;
uniform sampler2D u_normal;

uint random_int(inout uint state)
{
    uint a = 1664525u;
    uint c = 1013904223u;
    uint m = 2147483647u;
    
    state = (a * state + c) % m;
    
    return state;
}

float random_uniform_float(inout uint state)
{
	uint rint = random_int(state);
    return float(rint) / 4294967295.0;
}

float random_float(inout uint state)
{
	return random_uniform_float(state) * 2.0 - 1.0; 
}

const float PI = 3.14;

void main()
{
	vec3 normal = normalize(texture(u_normal, v_texCoord).xyz);
	vec3 position = texture(u_geometry, v_texCoord).xyz;

	float occulusion_hits = 0;
	uint state = uint(v_texCoord.x * 1920.0 + v_texCoord.y * 1080.0 * 1920.0);

	for(int i = 0; i < u_samples; i++)
	{
		float r = random_uniform_float(state);
		float theta = random_uniform_float(state) * 2 * PI;
		float phi = random_uniform_float(state) * PI;

		vec3 random_dir = normalize(r * vec3(sin(phi) * cos(theta), sin(phi) * cos(theta), cos(phi)));
		
		if(dot(random_dir, normal) < 0.0)
		{
			random_dir = -random_dir;
		}

		// in view space
		vec3 random_point = position + random_dir * random_uniform_float(state) * u_distance;

		//sample points depth 
		vec4 random_point_ndc = (u_projection * vec4(random_point, 1.0));

		vec2 random_point_tex_coord = ((random_point_ndc.xy/random_point_ndc.w) + vec2(1.0)) * 0.5;

		if(random_point_tex_coord.x < 0 || random_point_tex_coord.x >= 1 
		|| random_point_tex_coord.y < 0 || random_point_tex_coord.y >= 1)
		{
			continue;
		}

		float expected_depth = random_point.z;
		float actual_depth = texture(u_geometry, random_point_tex_coord).z;

		float rangeCheck = smoothstep(0.0, 1.0, u_distance / abs(expected_depth - actual_depth));
		occulusion_hits += (abs(actual_depth - expected_depth) > (u_distance-u_bias)  ? 1 : 0) * rangeCheck;
	
//		occulusion_hits += abs(actual_depth - expected_depth) < (u_distance-u_bias) ? 1 : 0;
	}

	float occulusion = occulusion_hits / float(u_samples);

	if(position.z == 0.0)
	{
		occulusion = 0.0;
	}

	o_ambientOcculusion = occulusion;
}