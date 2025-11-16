#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal_cameraspace;
out vec3 vertexPosition_cameraspace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Todo en espacio de la cámara para facilitar los cálculos de luz
    vertexPosition_cameraspace = (view * model * vec4(aPos, 1.0)).xyz;
    Normal_cameraspace = (view * model * vec4(aNormal, 0.0)).xyz;
    TexCoords = aTexCoords;
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}