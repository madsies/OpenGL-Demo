#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Colour;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 viewPos;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    //vec4 textureCol = texture(objTexture, TexCoord);

    float factor = clamp((FragPos.y + 32.0) / 64.0, 0.2, 1.0); 
    FragColor = vec4(Colour * factor, 1.0);

}