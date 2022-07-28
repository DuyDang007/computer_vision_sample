#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vCol;

void main()
{
    vec3 tmpPos = vec3(pos.x/5, pos.y, pos.z/10);
    gl_Position = projection * view * model * vec4(tmpPos/100, 1.0);
    vCol = vec3(color.x / 255, color.y / 255, color.z / 255);
}
