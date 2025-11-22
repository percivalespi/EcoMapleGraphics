#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform float alphaMultiplier; // <--- VARIABLE NUEVA

void main()
{
    vec4 texColor = texture(texture_diffuse1, TexCoords);
    // Multiplicamos el alpha de la imagen por nuestro multiplicador
    FragColor = vec4(texColor.rgb, texColor.a * alphaMultiplier);
}