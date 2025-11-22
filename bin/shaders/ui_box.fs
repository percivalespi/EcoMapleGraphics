#version 330 core
out vec4 FragColor;
in vec2 UV;

uniform vec4 bgColor;
uniform vec4 borderColor;
uniform float borderWidth; // Grosor del borde (0.0 a 0.5)

void main() {
    // Lógica matemática para dibujar el borde sin texturas
    // Si UV.x o UV.y están cerca de 0 o 1, es borde.
    
    if (UV.x < borderWidth || UV.x > 1.0 - borderWidth ||
        UV.y < borderWidth * 4.0 || UV.y > 1.0 - (borderWidth * 4.0)) // *4.0 para compensar el aspecto rectángulo
    {
        FragColor = borderColor;
    } else {
        FragColor = bgColor;
    }
}