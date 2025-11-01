#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

uniform sampler2D texture_diffuse1;

uniform vec4 LightColor;
uniform vec4 LightPower;
uniform int  alphaIndex;

uniform vec4 MaterialAmbientColor;
uniform vec4 MaterialSpecularColor;
uniform float transparency;

void main()
{    
    vec4 texel = texture(texture_diffuse1, TexCoords);

    // Alpha Testing
    if(texel.a < 0.5)
        discard;

    // Iluminación Phong
    vec3 n = normalize( Normal_cameraspace );
    vec3 l = normalize( LightDirection_cameraspace );
    vec3 E = normalize(EyeDirection_cameraspace);
    vec3 R = reflect(-l,n);

    // Componente Ambiental
    vec4 K_a = MaterialAmbientColor * LightColor;

    // Componente Difusa (Usa 'texel' como el color del material)
    float cosTheta = clamp( dot( n,l ), 0,1 );
    vec4 K_d = texel * LightColor * cosTheta;

    // Componente Especular
    float cosAlpha = clamp( dot(E,R), 0, 1 );
    vec4 K_s = MaterialSpecularColor * LightColor * pow(cosAlpha,alphaIndex);
    
    // Color final de la iluminación
    vec4 final_color = (K_a + K_d + K_s) * LightPower;
    final_color.a = transparency;

    FragColor = final_color;
}