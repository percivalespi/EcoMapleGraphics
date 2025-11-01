#version 330 core
out vec4 FragColor;

uniform vec4 crosshairColor; // Color de la cruz

void main()
{
    FragColor = crosshairColor;
}