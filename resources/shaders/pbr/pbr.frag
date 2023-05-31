#version 330 core

out vec4 o_colour;

in vec2 v_texCoord;

// deferred output
uniform sampler2D u_albedo;
uniform sampler2D u_geometry;
uniform sampler2D u_normal;
uniform sampler2D u_roughMetal;
uniform sampler2D u_emissive;
uniform sampler2D u_ambientOcculusion;

// environment 
uniform samplerCube u_irradianceEnvironmentMap;
uniform samplerCube u_prefilteredEnvironmentMap;
uniform sampler2D u_brdfLut;  

// light settings
uniform vec3 u_lightDirectionViewSpace;
uniform vec3 u_lightRadiance;

// camera settings
uniform mat3 u_inverseViewNormal;

const float PI = 3.14;
const float MAX_ENVIRONMENT_LOD = 4;

// (F) amount of each channel of light reflected at the viewing angle
vec3 FresnelSchlick(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// (D) normal distribution  of micro facets aligned to the half way vector
float DistributionGGX(float normalDotHalfway, float roughness)
{
    float a = roughness*roughness;
    float aSquared = a*a;
    
    return aSquared / (PI * pow(normalDotHalfway * normalDotHalfway * (aSquared - 1.0) + 1.0, 2.0));
}

float GeometrySchlickGGX(float normalDotToEye, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    return normalDotToEye / (normalDotToEye * (1.0 - k) + k);
}

// (G) distribution of geometry self shadowing
float GeometrySmith(float normalDotToEye, float normalDotToLight, float roughness)
{
    return GeometrySchlickGGX(normalDotToLight, roughness) * GeometrySchlickGGX(normalDotToEye, roughness);
}

// (Lo) sample outgoing light
vec3 LightOutgoing(float normalDotToEye, float normalDotToLight, float normalDotHalfway, vec3 kS, vec3 kD, vec3 albedo, float metallic, float roughness)
{
    // Specular contribution 
    float D = DistributionGGX(normalDotHalfway, roughness);       
    float G = GeometrySmith(normalDotToEye, normalDotToLight, roughness);   

    vec3 specular = (kS * D * G) / (4.0 * normalDotToEye * normalDotToLight + 0.0001);

    // Diffuse contribution
    vec3 diffuse = kD * (albedo / PI);

    return diffuse + specular * normalDotToLight;
}

void main()
{
    // Sample deferred buffers and acquire parameters
	vec3 albedo = texture(u_albedo, v_texCoord).rgb;
	vec3 geometry = texture(u_geometry, v_texCoord).xyz;
	vec3 roughMetal = texture(u_roughMetal, v_texCoord).xyz;
	vec3 emissive = texture(u_emissive, v_texCoord).xyz;
	vec3 normal = normalize(texture(u_normal, v_texCoord).xyz);

    vec3 toEye = normalize(-geometry);
    
    vec3 worldNormal = normalize(u_inverseViewNormal * normal);
    vec3 worldToEye = normalize(u_inverseViewNormal * toEye);

    float ambient = roughMetal.x;
	float roughness = roughMetal.y;
	float metallic = roughMetal.z;

    // Reflectance color at 0 degrees
    vec3 F0 = mix(vec3(0.04), albedo, metallic); 

    // Specular and diffuse factors
    vec3 kS = FresnelSchlick(max(dot(normal, toEye), 0.0), F0, roughness);
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

    // Directional light
    vec3 toLight = normalize(-u_lightDirectionViewSpace);
    vec3 halfway = normalize(toLight + toEye);
    float normalDotToEye = max(dot(normal, toEye), 0.0);
    float normalDotHalfway = max(dot(normal, halfway), 0.0);
    float normalDotToLight = max(dot(normal, toLight), 0.0);

    vec3 outgoingLight = LightOutgoing(normalDotToEye, normalDotToLight, normalDotHalfway, kS, kD, albedo, metallic, roughness);

    // Environment contribution
    vec3 irradiance = texture(u_irradianceEnvironmentMap, worldNormal).rgb;

    vec3 toEnvironment = normalize(reflect(-worldToEye, worldNormal));
    vec3 prefilteredColor = textureLod(u_prefilteredEnvironmentMap, toEnvironment, roughness * MAX_ENVIRONMENT_LOD).rgb;
    vec2 envBRDF = texture(u_brdfLut, vec2(normalDotToEye, roughness)).rg;
    vec3 envSpecular = prefilteredColor * (kS * envBRDF.x + envBRDF.y);

    // Total color
    vec3 environment = kD * irradiance * albedo + envSpecular;
    vec3 light = outgoingLight * u_lightRadiance;
    vec3 color = environment * (1.0 - texture(u_ambientOcculusion, v_texCoord).r) + light + emissive;

    // Handle skybox
    if(geometry.z == 0.0)
    {
        color = albedo;
    }

    o_colour = vec4(color, 1.0f);
}