#version 330 core
layout (location = 0) in vec3  aPos;
layout (location = 1) in vec3  aNormal;
layout (location = 2) in vec2  aTexCoords;

out vec2 TexCoords;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPosition;
uniform bool isTerrain; // <--- UNIFORM AÑADIDO

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    vec3 correctedNormal = aNormal;

    // --- CORRECCIÓN PARA EL TERRENO ---
    // Si el modelo es el terreno, rotamos su normal para que apunte hacia arriba de nuevo.
    if (isTerrain) {
        // Matriz de rotación de +90 grados en el eje X
        mat3 rotX = mat3(1.0, 0.0, 0.0,
                         0.0, 0.0, -1.0,
                         0.0, 1.0, 0.0);
        correctedNormal = rotX * aNormal;
    }
    // ------------------------------------

    // Transformamos la normal (ya corregida si era necesario) a camera space
    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    Normal_cameraspace = normalize(normalMatrix * correctedNormal);

    // El resto de los cálculos se mantienen
    vec3 vertexPosition_cameraspace = (view * model * vec4(aPos,1)).xyz;
    EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;
    vec3 LightPosition_cameraspace = ( view * vec4(lightPosition,1)).xyz;
    LightDirection_cameraspace = LightPosition_cameraspace - vertexPosition_cameraspace;
    
    TexCoords = aTexCoords;
}