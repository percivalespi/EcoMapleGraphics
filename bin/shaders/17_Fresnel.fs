#version 330 core

// Adaptation for OpenGL/GLSL by: PhD Sergio Teodoro-Vite
// Based on: The CG Tutorial, Chapter 7 (NVIDIA)

in vec2 TexCoords;

in vec3 vReflect;
in vec3 vRefract[3];
in float reflectionCoefficient;

out vec4 FragColor;

// Cubemap ambiental y (opcional) una textura difusa del modelo
uniform samplerCube cubetex;
uniform sampler2D   texture_diffuse1;

void main(void)
{
    // Muestreo del cubemap
    vec4 reflectedColor = texture(cubetex, vReflect);

    vec4 refractedColor = vec4(0.0);
    refractedColor.r = texture(cubetex, vRefract[0]).r;
    refractedColor.g = texture(cubetex, vRefract[1]).g;
    refractedColor.b = texture(cubetex, vRefract[2]).b;

    vec4 fresnelColor = reflectionCoefficient * reflectedColor +
                        (1.0 - reflectionCoefficient) * refractedColor;

    FragColor = vec4(fresnelColor.rgb, 1.0);
}
