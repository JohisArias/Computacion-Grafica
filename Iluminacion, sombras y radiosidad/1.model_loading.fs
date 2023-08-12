#version 330 core                      // versión 3.30 (OpenGL 3.3 Core Profile)

out vec4 FragColor;                    // para el color final del fragmento ( color de un píxel en la pantalla)

in vec2 TexCoords;                     // coordenadas de textura interpoladas desde el vertex shader
in vec3 Normal;
in vec3 color;
in vec3 crntPos;

uniform vec3 surfaceColor;             // para color del piso (superficie)
uniform bool useTexture;               // usar textura o color solido

uniform sampler2D texture_diffuse1;    // textura de difusión (color) del objeto

uniform vec3 viewPosition;             // posicion de camara

struct Light {
    vec3 position;
    vec4 color;
};

uniform Light lights[2]; // Array de luces (Light1 y Light2)

void main()
{
    // ambient lighting
    float ambient = 0.20f;
    
    // diffuse lighting
    vec3 normal = normalize(Normal);
    
    // specular lighting
    float specularLight = 0.50f;
    vec3 viewDirection = normalize(viewPosition - crntPos);

    vec3 resultColor = vec3(0.0);

    for (int i = 0; i < 2; ++i) {
        // diffuse lighting
        vec3 lightDirection = normalize(lights[i].position - crntPos);
        float diff = max(dot(normal, lightDirection), 0.0);

        // specular lighting
        vec3 reflectionDirection = reflect(-lightDirection, normal);
        float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
        float spec = specAmount * specularLight;

        // Calcular distancia entre el fragmento y la luz
        float distance = length(lights[i].position - crntPos);

        // Calcular atenuación usando coeficientes de atenuación
        float constant = 1.0;
        float linear = 0.09;
        float quadratic = 0.032;
        float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

        // Cálculo de luz ambiental, difusa y especular
        vec3 ambient = ambient * lights[i].color.rgb;
        vec3 diffuse = diff * lights[i].color.rgb;
        vec3 specular = spec * lights[i].color.rgb;

        // Aplicar atenuación a las componentes de iluminación
        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;

        // Sumar las componentes de iluminación al resultado final
        resultColor += ambient + diffuse + specular;
    }
    
    if (useTexture) {     // textura en objeto
        FragColor = texture(texture_diffuse1, TexCoords) * vec4(resultColor, 1.0);
    } else {               // sin textura para piso
        FragColor = vec4(surfaceColor, 1.0) * vec4(resultColor, 1.0);
    }
}