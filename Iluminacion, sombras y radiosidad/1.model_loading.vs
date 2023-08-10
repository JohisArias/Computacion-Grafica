#version 330 core                           // versión 3.30 (OpenGL 3.3 Core Profile)

// Input Attributes
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

// Output Variables
out vec2 TexCoords;                         // pasa las coordenadas de textura (aTexCoords) al fragment shader

// Uniform Matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}