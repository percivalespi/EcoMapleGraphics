#version 330 core
out vec4 FragColor;

in vec2 TexCoords; // <-- AÑADIR ESTA LÍNEA

uniform sampler2D texture_diffuse1; // <-- AÑADIR SAMPLER
uniform vec4 tintColor; // <-- AÑADIR FACTOR DE TINTE (reemplaza sunColor)

void main()
{
    // Muestrear la textura y multiplicarla por el tinte
    FragColor = texture(texture_diffuse1, TexCoords) * tintColor;
}