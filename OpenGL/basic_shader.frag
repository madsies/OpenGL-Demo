#version 330 core

in vec3 FragPos;
in vec3 Colour;

out vec4 FragColor;

void main()
{
    float factor = clamp((FragPos.y + 1.0) / 2.0, 0.0, 1.0); 
    FragColor = vec4(Colour * factor, 1.0);

}