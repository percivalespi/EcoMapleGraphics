#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

// Input for instancing
layout (location = 11) in mat4 instanceMatrix;

// Outputs to the fragment shader
out vec2 TexCoords;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;

// Uniforms from the C++ code
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPosition;

void main()
{
    // Transform vertex position and normal into camera space
    vec3 vertexPosition_cameraspace = (view * instanceMatrix * vec4(aPos, 1.0)).xyz;
    Normal_cameraspace = (view * instanceMatrix * vec4(aNormal, 0.0)).xyz;

    // Calculate light and eye direction vectors in camera space
    vec3 LightPosition_cameraspace = (view * vec4(lightPosition, 1.0)).xyz;
    LightDirection_cameraspace = LightPosition_cameraspace - vertexPosition_cameraspace;
    EyeDirection_cameraspace = vec3(0, 0, 0) - vertexPosition_cameraspace;

    // Standard outputs
    TexCoords = aTexCoords;
    gl_Position = projection * vec4(vertexPosition_cameraspace, 1.0);
}