#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;

// controles
uniform float waveStrength;   // 0.05 – 0.35
uniform float waveSpeed;      // 0.5 – 3.0

uniform float windStrength;   // 0.05 – 0.4
uniform float windSpeed;      // 0.5 – 3.0

uniform float noiseStrength;  // 0.02 – 0.15

float fakeNoise(vec3 p)
{
    return sin(p.x * 1.2 + time * 0.7) *
           cos(p.y * 1.7 + time * 0.4);
}

void main()
{
    // onda suave
    float wave = sin(aPos.y * 1.5 + time * waveSpeed);

    // viento que empuja lateralmente
    float wind = sin(time * windSpeed + aPos.y * 0.8);

    // ruido suave adicional para deformación
    float n = fakeNoise(aPos);

    vec3 pos = aPos;

    // Onda principal
    pos.x += wave * waveStrength;

    // Empuje del viento
    pos.x += wind * windStrength;

    // Deformación ligera tipo turbulencia
    pos.x += n * noiseStrength;
    pos.y += n * noiseStrength * 0.4;

    gl_Position = projection * view * model * vec4(pos, 1.0);
    TexCoords = aTexCoords;
}
