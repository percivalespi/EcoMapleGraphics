#version 330 core
layout (location = 0) in vec2 aPos; // Posición 2D en NDC (-1 a 1)

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); // Z=0, W=1
}