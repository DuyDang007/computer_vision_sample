#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vCol;

void main()
{
    vec3 tmpPos = vec3(pos.x, -pos.y, -pos.z);
    gl_Position = projection * view * model * vec4(pos.x/100.0, -pos.y/100.0, -pos.z/20.0, 1.0);
    vCol = vec3(color.x / 255, color.y / 255, color.z / 255);
}
