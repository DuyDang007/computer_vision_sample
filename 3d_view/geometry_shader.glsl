#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 vCol[];
out vec3 gCol;


void main()
{
    gCol = vCol[0];
    gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.03, 0.0, 1.0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(-0.03, -0.015, 0.0, 1.0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(0.03, -0.015, 0.0, 1.0);
    EmitVertex();

    EndPrimitive();
}