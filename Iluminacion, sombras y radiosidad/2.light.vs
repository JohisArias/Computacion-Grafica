#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 camMatrix;

uniform vec3 LightPosition1; // Posición de la primera luz
uniform vec3 LightPosition2; // Posición de la segunda luz

out vec3 FragPos; // Variable de salida para la posición del fragmento

void main()
{
    gl_Position = camMatrix * model * vec4(aPos, 1.0f);
    FragPos = vec3(model * vec4(aPos, 1.0));
}