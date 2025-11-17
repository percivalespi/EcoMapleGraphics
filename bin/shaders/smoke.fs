#version 330 core

out vec4 FragColor;

uniform vec4 baseColor; // por ejemplo (0.2,0.2,0.2,0.5)

void main()
{
    FragColor = baseColor;
}
