#version 330 core
out vec4 FragColor;

uniform vec3 chColor;

void main()
{
    FragColor = vec4(chColor, 1.0);
}