#version 330 core
out vec4 FragColor;

in vec3 WorldPos;

uniform sampler2D panorama;

vec2 sampleEquirect(vec3 dir)
{
    float phi = atan(dir.z, dir.x);
    float theta = asin(dir.y);
    vec2 uv = vec2((phi + 3.1415926) / (2.0 * 3.1415926),
                   (theta + 3.1415926/2.0) / 3.1415926);
    return uv;
}

void main()
{
    vec2 uv = sampleEquirect(normalize(WorldPos));
    FragColor = texture(panorama, uv);
}