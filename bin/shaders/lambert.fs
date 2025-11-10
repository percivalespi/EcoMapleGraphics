#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal_cameraspace;
in vec3 vertexPosition_cameraspace;

uniform sampler2D texture_diffuse1;
uniform vec4 MaterialAmbientColor;
uniform vec4 MaterialDiffuseColor;
uniform float transparency;

#define MAX_LIGHTS 10
uniform int numLights;

uniform struct Light {
    vec3 Position;
    vec4 Color;
    vec4 Power;
    float distance;
    // Nota: No necesitamos 'Direction' ni 'alphaIndex' para Lambert básico
} allLights[MAX_LIGHTS];

uniform mat4 view; // Necesario para pasar la posición de la luz a espacio cámara

void main()
{
    vec3 n = normalize(Normal_cameraspace);
    vec4 total_light = vec4(0.0);
    // En el Fragment Shader

    for(int i = 0; i < numLights; ++i) {
        vec3 LightPos_cam = (view * vec4(allLights[i].Position, 1.0)).xyz;
        vec3 LightDir_cam = LightPos_cam - vertexPosition_cameraspace;
        float dist = length(LightDir_cam);
        vec3 l = normalize(LightDir_cam);

        float attenuation = 1.0 / (1.0 + allLights[i].distance * dist * dist * 0.01); // Atenuación cuadrática suave

        // --- Componente Ambiental ---
        vec4 ambient = MaterialAmbientColor * allLights[i].Color;

        // --- Componente Difusa (Lambert) ---
        float diff = max(dot(n, l), 0.0);
        vec4 diffuse = diff * MaterialDiffuseColor * allLights[i].Color;

        total_light += (ambient + diffuse) * allLights[i].Power * attenuation;
    }

    total_light.a = transparency;
    FragColor = texture(texture_diffuse1, TexCoords) * total_light;
}