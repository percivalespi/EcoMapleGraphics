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

    vec3 n = normalize( Normal_cameraspace );
    vec3 l = normalize( LightDirection_cameraspace );
    vec3 E = normalize(EyeDirection_cameraspace);
    vec3 R = reflect(-l,n);

    // 1. Componente Ambiental (luz de relleno global)
    vec4 ambient = MaterialAmbientColor;

    // 2. Componente Difusa (luz directa del sol)
    float diff = max(dot(n, l), 0.0);
    vec4 diffuse = LightColor * diff;

    // 3. Componente Especular (brillo de la fuente de luz)
    float spec = pow(max(dot(E, R), 0.0), float(alphaIndex));
    vec4 specular = MaterialSpecularColor * LightColor * spec;
    
    // 4. Color Final: La luz (ambiente + difusa) ilumina la textura, y el brillo se suma encima.
    vec4 final_color = (ambient + diffuse * LightPower) * texel + (specular * LightPower);
    final_color.a = texel.a * transparency;

    FragColor = final_color;
}