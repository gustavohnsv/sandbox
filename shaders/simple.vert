#version 330 core
layout (location = 0) in vec3 aPos; // Pega apenas a primeira localização de atributo (posição)

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Ignora normais, UVs, tudo. Apenas transforma a posição.
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}