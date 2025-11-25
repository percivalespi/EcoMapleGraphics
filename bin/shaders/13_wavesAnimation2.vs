#version 330 core
layout (location = 0) in vec3  aPos;
layout (location = 1) in vec3  aNormal;
layout (location = 2) in vec2  aTexCoords;

out vec2 TexCoords;
out vec2 vTex;
out vec3 vNormalW;
out vec3 vWorldPos;
out float vWave;  

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;
uniform float radius;
uniform float height;

uniform vec3 center1;  // centro #1 en coords de mundo (glm::vec3)
uniform vec3 center2;  // centro #2 en coords de mundo (glm::vec3)

float radialWave(vec3 worldPos, vec3 c, float rad, float t)
{
    // Distancia en el plano del agua (ignora diferencia de altura)
    vec2 p  = worldPos.xz;
    vec2 pc = c.xz;
    float d = length(p - pc);

    // Fase: 2π * (d / rad) - ωt  (ω=1 para simplicidad)
    float phase = 6.2831853 * (d / max(rad, 1e-4)) - t;

    // Amplitud con atenuación simple (1 / (1 + d / rad))
    float atten = 1.0 / (1.0 + d / max(rad, 1e-4));

    return sin(phase) * atten;
}

void main()
{
    vec4 world = model * vec4(aPos, 1.0);
    
    // Dos contribuciones independientes, misma radius/height
    float w1 = radialWave(world.xyz, center1, radius, time);
    float w2 = radialWave(world.xyz, center2, radius, time);

    float disp = height * (w1 + w2);   // suma de ondas

    // Desplazamos SOLO en Y (vertical) para simular ola
    world.y += disp;

    // Aproxima normal en mundo: mantiene la original transformada (simple)
    mat3 nmat = mat3(transpose(inverse(model)));
    vNormalW  = normalize(nmat * aNormal);

    TexCoords      = aTexCoords;
    vWorldPos = world.xyz;
    vWave     = disp;

    gl_Position = projection * view * world;
}