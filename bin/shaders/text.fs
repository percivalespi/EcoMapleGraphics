#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main() {
    // Muestreamos el color de la textura. 
    // Si usas una textura con fondo negro y letras blancas:
    vec4 sampled = texture(text, TexCoords);
    
    // Si la textura tiene fondo negro, usamos el canal rojo (o promedio) como alpha
    float alpha = (sampled.r + sampled.g + sampled.b) / 3.0;
    
    if(alpha < 0.1) discard; // Quitar el fondo negro de la letra
    
    color = vec4(textColor, 1.0);
}