#version 330 core

out vec3 o_color;

uniform sampler2D u_equirectangularMap;

in vec3 v_position;

const vec2 INVERSE_ATAN = vec2(0.1591, 0.3183);

vec2 SphereUV(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= INVERSE_ATAN;
    uv += 0.5;

    return uv;
}

void main()
{		
    vec2 uv = SphereUV(normalize(v_position)); 
    
    o_color = texture(u_equirectangularMap, uv).rgb;
}