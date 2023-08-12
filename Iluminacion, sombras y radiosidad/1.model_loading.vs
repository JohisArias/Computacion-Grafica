#version 330 core                           // versión 3.30 (OpenGL 3.3 Core Profile)

// Input Attributes
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aColor;

// Output Variables
out vec2 TexCoords;                         // pasa las coordenadas de textura (aTexCoords) al fragment shader
out vec3 color;
out vec3 Normal;
out vec3 crntPos;							// Outputs the current position for the Fragment Shader

out vec3 FragColor;                         // color interpolado para el fragment shader
out vec3 FragPos;                           // posición interpolada para el fragment shader

// Uniform Matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 camMatrix;

void main()
{
    // calculates current position
	crntPos = vec3(model * vec4(aPos, 1.0f));
	// Outputs the positions/coordinates of all vertices
	gl_Position = camMatrix * vec4(crntPos, 1.0);

	// Assigns the colors from the Vertex Data to "color"
	color = aColor;
	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	TexCoords = aTexCoords;
	// Assigns the normal from the Vertex Data to "Normal"
	Normal = aNormal;
}