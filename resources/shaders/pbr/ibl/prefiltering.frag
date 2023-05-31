#version 330 core

out vec3 o_color;

uniform samplerCube u_environmentMap;
uniform float u_roughness;

in vec3 v_position;

const float PI = 3.14159265359;
const uint SAMPLE_COUNT = 1024u;

float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint currentSample, uint samples)
{
    return vec2(float(currentSample)/float(samples), RadicalInverse_VdC(currentSample));
}

vec3 ImportanceSampleGGX(vec2 randomCoordinate, mat3 tbn, float roughness)
{
    float a = roughness*roughness;
	
    // spherical coordinates aligned with specular lobe
    float phi = 2.0 * PI * randomCoordinate.x;
    float cosTheta = sqrt((1.0 - randomCoordinate.y) / (1.0 + (a*a - 1.0) * randomCoordinate.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    vec3 randomHalfway = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
	
    return normalize(tbn * randomHalfway);
}    

void main()
{		
    vec3 normal = normalize(v_position);    
    vec3 toEnvironment = normal;
    vec3 toEye = toEnvironment;

    vec3 up = abs(normal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, normal));
    vec3 bitangent = cross(normal, tangent);

    mat3 tbn = mat3(tangent, bitangent, normal);

    float totalWeight = 0.0;   
    vec3 prefilteredColor = vec3(0.0);     

    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 randomCoordinate = Hammersley(i, SAMPLE_COUNT);
        vec3 randomHalfway = ImportanceSampleGGX(randomCoordinate, tbn, u_roughness);
        vec3 toLight = normalize(reflect(-toEye, randomHalfway));

        float normalDotToLight = max(dot(normal, toLight), 0.0);

        if(normalDotToLight > 0.0)
        {
            prefilteredColor += texture(u_environmentMap, toLight).rgb * normalDotToLight;
            totalWeight += normalDotToLight;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;

    o_color = prefilteredColor;
} 