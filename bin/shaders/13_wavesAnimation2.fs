#version 330 core
out vec4 FragColor;

in vec3  vNormalW;
in vec3  vWorldPos;
in float vWave;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform float time;
uniform float radius;
uniform vec3 center1;
uniform vec3 center2;
uniform float texFlowScale;

vec2 radialFlow(vec3 worldPos, vec3 c, float rad, float t)
{
    vec2 p  = worldPos.xz;
    vec2 pc = c.xz;
    vec2 v  = p - pc;
    float d = length(v) + 1e-4;
    vec2 dir = v / d; // dirección radial

    // Usamos cos(phase) para que el flujo “empuje” al compás de la onda
    float phase = 6.2831853 * (d / max(rad, 1e-4)) - t;
    float atten = 1.0 / (1.0 + d / max(rad, 1e-4));

    float mag = cos(phase) * atten; // intensidad local del flujo
    
    return dir * mag;
}

void main()
{
    // Distorsión de UV sumando flujos de ambos centros
    vec2 flow = radialFlow(vWorldPos, center1, radius, time)
              + radialFlow(vWorldPos, center2, radius, time);

<<<<<<< Updated upstream
    vec2 coordsT = TexCoords;
    coordsT.x += 0.01f*time;
=======
    vec2 uvDistorted = TexCoords + texFlowScale * flow;
>>>>>>> Stashed changes

    // Muestrea textura con UV distorsionado
    vec4 tex = texture(texture_diffuse1, uvDistorted);

    // Iluminación lambert sencilla (opcional)
    vec3 L = normalize(vec3(0.2, 1.0, 0.2));
    float ndl = clamp(dot(normalize(vNormalW), L), 0.0, 1.0);

    // Ligera variación con la altura instantánea de la ola (vWave)
    float k = clamp(0.5 + 2.0 * vWave, 0.0, 1.0);

    vec3 color = tex.rgb * (0.35 + 0.65 * ndl);
    color = mix(color * 0.9, color * 1.1, k);

    FragColor = vec4(color, tex.a);
}