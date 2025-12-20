#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColour;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Colour;

void main()
{
	vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    Colour = aColour;

    gl_Position = projection * view * worldPos;
}