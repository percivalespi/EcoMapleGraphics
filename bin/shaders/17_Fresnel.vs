#version 330 core

// Adaptation for OpenGL/GLSL by: PhD Sergio Teodoro-Vite
// Based on: The CG Tutorial, Chapter 7 (NVIDIA)

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;  // tu Model ya provee texcoords

out vec2 TexCoords;

// Outgoing Fresnel reflection and refraction parameters
out vec3 vReflect;
out vec3 vRefract[3];
out float reflectionCoefficient;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPosition;

// Fresnel parameters (expuestos para ajustar apariencia)
uniform float mRefractionRatio; // ~1.0/1.52 para vidrio ~0.658
uniform float _Bias;            // típico 0.02–0.1
uniform float _Scale;           // típico 0.9–1.0
uniform float _Power;           // típico 5.0

void main(void)
{
    vec4 in_Position = vec4(aPos, 1.0);
    gl_Position = projection * view * model * in_Position;

    // Posición en mundo
    vec3 posWorld  = (model * in_Position).xyz;

    // Normal en mundo (3x3 de las 3 primeras columnas del model)
    mat3 nMat = mat3(model);
    vec3 normWorld = normalize(nMat * aNormal);

    // I: vector desde el ojo hacia el punto de la superficie
    vec3 I = normalize(posWorld - cameraPosition);

    // Reflexión y refracción (tres índices ligeramente distintos para RGB)
    vReflect    = reflect(I, normWorld);
    vRefract[0] = refract(I, normWorld, mRefractionRatio * 1.00); // R
    vRefract[1] = refract(I, normWorld, mRefractionRatio * 0.99); // G
    vRefract[2] = refract(I, normWorld, mRefractionRatio * 0.98); // B

    // Schlick Approximation
    float f = clamp(_Bias + _Scale * pow(1.0 + dot(I, normWorld), _Power), 0.0, 1.0);
    reflectionCoefficient = f;

    TexCoords = aTexCoords;
}