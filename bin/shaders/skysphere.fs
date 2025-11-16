#version 330 core
out vec4 FragColor;

in vec3 WorldPos;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183); // (1/2pi, 1/pi)

vec2 SampleSphericalMap(vec3 v)
{
    // Convierte vector dirección a coordenadas UV
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{       
    vec2 uv = SampleSphericalMap(normalize(WorldPos));
    
    // Muestreamos la textura HDR
    vec3 color = texture(equirectangularMap, uv).rgb;
    
    // Tone mapping básico (HDR -> LDR) para que no se vea "quemado"
    // Si se ve muy oscuro o brillante, ajusta aquí
    color = color / (color + vec3(1.0));
    
    // Gamma correction
    color = pow(color, vec3(1.0/2.2)); 
    
    FragColor = vec4(color, 1.0);
}