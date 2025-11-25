#version 330 core
layout (location = 0) in vec3  aPos;
layout (location = 1) in vec3  aNormal;
layout (location = 2) in vec2  aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;
uniform float height;

void main()
{
    
    vec4 PosL = vec4(aPos, 1.0f);
    //PosL.z += height * sin(PosL.x + time);
    PosL.z += height * sin(PosL.y + time);

    gl_Position = projection * view * model * PosL;

    TexCoords = aTexCoords;  
}