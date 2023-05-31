#version 330 core

out vec2 o_color;

in vec2 v_texCoord;

const float PI = 3.14159265359;

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

float GeometrySchlickGGX(float normalDotToEye, float roughness)
{
    float a = roughness;
    float k = (a * a) / 2.0;

    return normalDotToEye / (normalDotToEye * (1.0 - k) + k);
}

float GeometrySmith(float normalDotToEye, float normalDotToLight, float roughness)
{
    return GeometrySchlickGGX(normalDotToLight, roughness) *  GeometrySchlickGGX(normalDotToEye, roughness);
}  

void IntegrateBRDF(float normalDotToEye, float roughness, out float scale, out float bias)
{
    vec3 toEye = vec3(
        sqrt(1.0 - normalDotToEye*normalDotToEye),
        0.0,
        normalDotToEye);

    scale = 0.0;
    bias = 0.0;

    vec3 normal = vec3(0.0, 0.0, 1.0);
    vec3 tangent = vec3(1.0, 0.0, 0.0);
    vec3 bitangent = vec3(0.0, 1.0, 0.0);
    mat3 tbn = mat3(tangent, bitangent, normal);

    const uint SAMPLE_COUNT = 1024u;
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 randomCoordinate = Hammersley(i, SAMPLE_COUNT);
        vec3 randomHalfway = ImportanceSampleGGX(randomCoordinate, tbn, roughness);
        vec3 toLight = normalize(reflect(-toEye, randomHalfway));

        float normalDotToLight = max(toLight.z, 0.0);
        float normalDotHalfway = max(randomHalfway.z, 0.0);
        float toEyeDotHalfway = max(dot(toEye, randomHalfway), 0.0);

        if(normalDotToLight > 0.0)
        {
            float G = GeometrySmith(normalDotToEye, normalDotToLight, roughness);
            float G_Vis = (G * toEyeDotHalfway) / (normalDotHalfway * normalDotToEye);
            float Fc = pow(1.0 - toEyeDotHalfway, 5.0);

            scale += (1.0 - Fc) * G_Vis;
            bias += Fc * G_Vis;
        }
    }
    
    scale /= float(SAMPLE_COUNT);
    bias /= float(SAMPLE_COUNT);
}

void main() 
{
    float normalDotToEye = v_texCoord.x;
    float roughness = v_texCoord.y;

    float scale;
    float bias;
    IntegrateBRDF(normalDotToEye, roughness, scale, bias);

    o_color = vec2(scale, bias);
}