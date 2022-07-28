#version 330 core

in vec3 gCol;
out vec4 FragColor;

void main()
{
    FragColor = vec4(gCol, 1.0);
}
