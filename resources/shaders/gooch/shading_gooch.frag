#version 330 core

out vec4 o_colour;

in vec2 v_texCoord;

uniform sampler2D u_albedo;
uniform sampler2D u_geometry;
uniform sampler2D u_normal;
uniform sampler2D u_temperature;

uniform vec3 u_lightDirection;

uniform int u_scale;
uniform float u_depthThreshold;
uniform float u_normalThreshold;

uniform float u_depthNormalThreshold;
uniform float u_depthNormalThresholdScale;

void edge_depth(out float depthBL, out float edgeDepth, out float edgeNormal)
{
	vec2 texelSize = 1.0f / textureSize(u_geometry, 0);

	float halfScaleFloor = floor(u_scale/2.0f);
	float halfScaleCeil = ceil(u_scale/2.0f);

	int d = 1;

	vec2 coordBL = v_texCoord - texelSize * halfScaleFloor;
	vec2 coordTR = v_texCoord + texelSize * halfScaleFloor;
	vec2 coordBR = v_texCoord + texelSize * vec2(halfScaleCeil, -halfScaleFloor);
	vec2 coordTL = v_texCoord + texelSize * vec2(-halfScaleFloor, halfScaleCeil);

	depthBL = texture(u_geometry, coordBL).z;
	float depthTR = texture(u_geometry, coordTR).z;
	float depthBR = texture(u_geometry, coordBR).z;
	float depthTL = texture(u_geometry, coordTL).z;

	float differenceBLTR = depthBL - depthTR;
	float differenceBRTL = depthBR - depthTL;
	edgeDepth = sqrt(pow(differenceBLTR, 2.0f) + pow(differenceBRTL, 2.0f));

	vec3 normalBL = texture(u_normal, coordBL).rgb;
	vec3 normalTR = texture(u_normal, coordTR).rgb;
	vec3 normalBR = texture(u_normal, coordBR).rgb;
	vec3 normalTL = texture(u_normal, coordTL).rgb;

	vec3 differenceNormalBLTR = normalBL - normalTR;
	vec3 differenceNormalBRTL = normalBR - normalTL;
	edgeNormal = sqrt(dot(differenceNormalBLTR,differenceNormalBLTR) + dot(differenceNormalBRTL, differenceNormalBRTL));
}

bool is_edge(vec3 toCamera, vec3 normal)
{
	float ndotv = 1.0f - dot(toCamera, normal);

	float normalThreshold01 = clamp((ndotv - u_depthNormalThreshold) / (1.0f - u_depthNormalThreshold), 0.0f, 1.0f);
	float normalThreshold = normalThreshold01 * u_depthNormalThresholdScale + 1.0f;

	float depthBL;
	float edgeDepth;
	float edgeNormal;
	edge_depth(depthBL, edgeDepth, edgeNormal);
	
	float depthThreshold = u_depthThreshold * depthBL * normalThreshold;

	return (abs(edgeDepth) > abs(depthThreshold) || edgeNormal > u_normalThreshold);
}

vec3 gooch_shade(vec3 albedo, float brightness, float b, float y, float alpha, float beta)
{
    vec3 warmColor = vec3(y, y, 0.0f) + alpha * albedo;
    vec3 coldColor = vec3(0.0f, 0.0f, b) + beta * albedo;

    float t = clamp((brightness + 1.0f) / 2.0f, 0.0f, 1.0f);

    vec3 color = mix(coldColor, warmColor, t);

	return color;
}

vec3 tone_map(vec3 color)
{
    color = color / (color + vec3(1.0f));
    color = pow(color, vec3(1.0f/2.2f)); 

    return color;
}

void main()
{
	vec3 albedo = texture(u_albedo, v_texCoord).rgb;
	vec3 normal = normalize(texture(u_normal, v_texCoord).rgb);
	vec4 temperature = texture(u_temperature, v_texCoord);
	vec3 viewSpacePosition = texture(u_geometry, v_texCoord).xyz;
	float depth = viewSpacePosition.z;

	vec3 toCamera = normalize(-viewSpacePosition.xyz);

	float blue = temperature.r;
	float yellow = temperature.g;
	float alpha = temperature.b;
	float beta = temperature.a;

    float ndotl = dot(normal, normalize(-u_lightDirection));

	vec3 color = gooch_shade(albedo, ndotl, blue, yellow, alpha, beta);
	
	float ndotv = 1.0f - dot(toCamera, normal);

	float normalThreshold01 = clamp((ndotv - u_depthNormalThreshold) / (1.0f - u_depthNormalThreshold), 0.0f, 1.0f);
	float normalThreshold = normalThreshold01 * u_depthNormalThresholdScale + 1.0f;
	
	if(is_edge(toCamera, normal))
	{
		color = vec3(0.0f);
	}

	if(depth >= 0.0f)
	{
		color = albedo;
	}

	o_colour = vec4(tone_map(color), 1.0f);
}