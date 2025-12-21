#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColour;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoord;
layout (location = 4) in mat4 instanceModel; // for instanced
uniform bool useInstance; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

mat4 finalModel; 

out vec3 FragPos;
out vec3 Colour;
out vec3 Normal;
out vec2 TexCoord;


void main()
{
    finalModel = (useInstance) ? instanceModel : model;

	vec4 worldPos = finalModel * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    Normal = mat3(transpose(inverse(finalModel))) * aNormal; 
    Colour = aColour;
    TexCoord = aTexCoord;

    gl_Position = projection * view * worldPos;
}