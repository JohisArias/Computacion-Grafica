#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 camMatrix;

uniform vec3 LightPosition1;            // Valor de la posición de la primera luz pasado desde el main
uniform vec3 LightPosition2;            // Valor de la posición de la segunda luz pasado desde el main

out vec3 FragPos;                       // Variable de salida para la posición del fragmento

void main()
{
    gl_Position = camMatrix * model * vec4(aPos, 1.0f);
    FragPos = vec3(model * vec4(aPos, 1.0));
}