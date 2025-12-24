#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in mat4 instanceModel; // per-instance

uniform mat4 model;
uniform mat4 lightSpaceMatrix;
uniform bool useInstance;

void main()
{
    mat4 finalModel = useInstance ? instanceModel : model;
    gl_Position = lightSpaceMatrix * finalModel * vec4(aPos, 1.0);
}