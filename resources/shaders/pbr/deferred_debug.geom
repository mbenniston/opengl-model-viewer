#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 8) out;

in VS_OUT {
    vec3 position;
    vec3 normal;
} gs_in[];

const float LINE_LENGTH = 0.01;
  
uniform mat4 u_projection;

out vec3 v_position;
out vec3 v_normal;

void generate_line(vec3 viewSpaceStart, vec3 viewSpaceEnd, vec3 normal)
{
    gl_Position = u_projection * vec4(viewSpaceStart, 1.0);
    v_position = viewSpaceStart;
    v_normal = normal;
    EmitVertex();
    
    gl_Position = u_projection * vec4(viewSpaceEnd, 1.0);
    v_position = viewSpaceStart;
    v_normal = normal;
    EmitVertex();

    EndPrimitive();
}

void main()
{
    vec3 center = gs_in[0].position +  gs_in[1].position +  gs_in[2].position;
    center /= 3.0f;

    vec3 normal = normalize(gs_in[0].normal +  gs_in[1].normal +  gs_in[2].normal);

    generate_line(center, center + normal * LINE_LENGTH, normal);

    generate_line(gs_in[0].position, gs_in[1].position, normal);
    generate_line(gs_in[1].position, gs_in[2].position, normal);
    generate_line(gs_in[2].position, gs_in[0].position, normal);
}