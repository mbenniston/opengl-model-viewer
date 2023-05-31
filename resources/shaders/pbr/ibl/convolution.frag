#version 330 core

out vec3 o_color;

uniform samplerCube u_environmentMap;

in vec3 v_position;

const float PI = 3.14159265359;

void main()
{		
    vec3 normal = normalize(v_position);
	vec3 bitangent = vec3(0.0, 1.0, 0.0);
	vec3 tangent = normalize(cross(bitangent, normal));
	bitangent = normalize(cross(normal, tangent));

	mat3 tbn = mat3(tangent, bitangent, normal);

    vec3 irradiance = vec3(0.0);
	float sampleDelta = 0.0125;
	float nrSamples = 0.0; 

	for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));

			vec3 sampleVec = tbn * tangentSample; 

			irradiance += texture(u_environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}

	irradiance = PI * irradiance * (1.0 / float(nrSamples));
  
    o_color = irradiance;
}
