#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;

uniform float intensity;   // 2.0 – 8.0
uniform float speed;       // 3.0 – 10.0
uniform float scale;       // 1.0 – 4.0

out vec3 FragPos;

// Nuevo nombre (no genera conflicto)
float fastNoise(vec3 p)
{
    return 
        sin(p.x * 3.7 + time * speed) +
        sin(p.y * 4.3 + time * speed * 1.2) +
        sin(p.z * 5.1 + time * speed * 0.8);
}

void main()
{
    vec3 pos = aPos;

    float n = fastNoise(pos * scale);

    pos.x += n * intensity * 0.7;
    pos.y += n * intensity;
    pos.z += n * intensity * 0.9;

    FragPos = vec3(model * vec4(pos, 1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
