#version 330 core                      // versi�n 3.30 (OpenGL 3.3 Core Profile)

out vec4 FragColor;                    // para el color final del fragmento ( color de un p�xel en la pantalla)

in vec2 TexCoords;                     // coordenadas de textura interpoladas desde el vertex shader

uniform sampler2D texture_diffuse1;    // textura de difusi�n (color) del objeto

void main()
{   
	// toma el color de la textura en las coordenadas interpoladas, y lo asigna al fragment
    FragColor = texture(texture_diffuse1, TexCoords);
}