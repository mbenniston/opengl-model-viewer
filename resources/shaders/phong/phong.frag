#version 330 core

layout(location=0) out vec3 o_color;

in vec3 v_position;
in vec3 v_normal;
in vec2 v_texcoord;

uniform sampler2D u_base;
uniform sampler2D u_emissive;

uniform vec4 u_baseFactor;
uniform vec3 u_emissiveFactor;

uniform vec3 u_lightDirection;
uniform vec3 u_lightColor;

uniform vec3 u_ambient;
uniform float u_specularPower;
uniform float u_specularStrength;

void main()
{
    vec3 albedo = pow(texture(u_base, v_texcoord).rgb, vec3(2.2)) * u_baseFactor.rgb;
    vec3 emissive = pow(texture(u_emissive, v_texcoord).rgb, vec3(2.2)) * u_emissiveFactor;
	
    vec3 normal = normalize(v_normal);
    vec3 toEye = normalize(-v_position);
    vec3 toLight = normalize(-u_lightDirection);
    vec3 halfway = normalize(toLight + toEye);

    float normalDotToLight = max(dot(normal, toLight), 0.0f);
    float halfwayDotToLight = max(dot(halfway, normal), 0.0f);

    vec3 diffuse = u_lightColor * normalDotToLight;
    vec3 specular =u_lightColor * u_specularStrength * pow(halfwayDotToLight, u_specularPower);
    vec3 color = emissive + albedo * (u_ambient + diffuse  + specular);

	color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 

	o_color = color;
}