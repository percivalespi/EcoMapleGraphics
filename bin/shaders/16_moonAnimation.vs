#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Órbita
uniform float time;    // controlado con ↑ / ↓ en tu C++
uniform float radius;  // radio de órbita, ej. 5.0
uniform float height;  // bamboleo/inclinación vertical leve

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
    // Desplazamiento orbital en torno al origen (Tierra en el origen)
    vec3 orbitOffset = vec3(
        radius * cos(time),
        0.2 * height * sin(2.0 * time),
        radius * sin(time)
    );

    // Posición y normal en mundo
    vec4 worldPos = model * vec4(aPos, 1.0);
    worldPos.xyz += orbitOffset;

    mat3 normalMat = transpose(inverse(mat3(model)));
    Normal   = normalize(normalMat * aNormal);
    FragPos  = worldPos.xyz;
    TexCoords = aTex;

    gl_Position = projection * view * worldPos;
}

