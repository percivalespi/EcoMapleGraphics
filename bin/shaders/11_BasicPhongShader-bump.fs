#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 ex_N; 

in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec3 Normal_cameraspace;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;

uniform vec4 LightColor;
uniform vec4 LightPower;
uniform int  alphaIndex;
uniform float distance;

uniform vec4 MaterialAmbientColor;
uniform vec4 MaterialDiffuseColor;
uniform vec4 MaterialSpecularColor;
uniform float transparency;

void main()
{    
    vec3 normTex = texture(texture_normal1, TexCoords).xyz;

    // Cálculo de componente difusa
    vec3 n = normalize( normTex );
    vec3 l = normalize( LightDirection_cameraspace );

    // Cálculo de componente ambiental
    vec4 K_a = MaterialAmbientColor * LightColor; // k_a_l = k_d_l = k_s_l = lightcolor

    // Cálculo de la componente difusa
    float cosTheta = clamp( dot( n,l ), 0,1 ); //l es el vector que se direje a la fuente de ilimunacion y regresa el al producto punto 
    vec4 K_d = MaterialDiffuseColor * LightColor * cosTheta; // k_a_o * k_a_l * (N*L) como son vectores unitarios solos e pone 1 

    // Cálculo de componente especular
    vec3 E = normalize(EyeDirection_cameraspace);// lo del ojo del espectador 
    vec3 R = reflect(-l,n); //vector reflejado, reflect calcula la reflexion entre dos vectores 
    float cosAlpha = clamp( dot(E,R), 0, 1 );
    vec4 K_s = MaterialSpecularColor * LightColor * pow(cosAlpha,alphaIndex); //k_s_o * k_s_l * (V*R) ^ q
    //                                                  (V*R)        q
    // Cálculo de ilumimación de Phong
    vec4 ex_color = K_a  * LightPower / (distance*distance) +
                    K_d  * LightPower / (distance*distance) +
                    K_s  * LightPower / (distance*distance);
                    
    ex_color.a = transparency;

    vec4 texel = texture(texture_diffuse1, TexCoords);

    FragColor = texel * ex_color;
   //FragColor = (K_a + K_d+ k_s)* LightPower / (distance*distance);
}