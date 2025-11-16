#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 WorldPos;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    WorldPos = aPos;
    
    // Quitamos la traslación de la vista para que el cielo no se mueva al caminar
    mat4 rotView = mat4(mat3(view)); 
    
    vec4 clipPos = projection * rotView * vec4(WorldPos, 1.0);

    // Truco de optimización: Z = W garantiza que la profundidad sea máxima (1.0)
    gl_Position = clipPos.xyww;
}