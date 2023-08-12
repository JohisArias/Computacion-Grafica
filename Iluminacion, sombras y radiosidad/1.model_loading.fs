#version 330 core                      // versión 3.30 (OpenGL 3.3 Core Profile)

out vec4 FragColor;                    // para el color final del fragmento ( color de un píxel en la pantalla)

in vec2 TexCoords;                     // coordenadas de textura interpoladas desde el vertex shader
in vec3 FragPos;
in vec3 Normal;
in vec3 color;
in vec3 crntPos;

uniform vec3 surfaceColor;             // para color del piso (superficie)
uniform bool useTexture;               // usar textura o color solido

uniform sampler2D texture_diffuse1;    // textura de difusión (color) del objeto

uniform vec3 viewPosition;             // posicion de camara

uniform vec4 lightColor;               // color of the light from the main function
// Luz 1
uniform vec3 Light1Pos;

void main()
{
    // ambient lighting
    float ambient = 0.20f;
    
    // diffuse lighting
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(Light1Pos - crntPos);
    float diffuse = max(dot(normal, lightDirection), 0.0f);
    
    // specular lighting
    float specularLight = 0.50f;
    vec3 viewDirection = normalize(viewPosition - crntPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
    float specular = specAmount * specularLight;

    float phongLighting = ambient + diffuse + specular;

    if(useTexture){     // textura en objeto
        FragColor = texture(texture_diffuse1, TexCoords) * lightColor * phongLighting;
   }
   else {               // sin textura para piso
        FragColor = vec4(surfaceColor, 1.0) * lightColor * phongLighting;        
   }
}