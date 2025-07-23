#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture_diffuse1;
uniform vec3 lineColor;

void main()
{
    FragColor = vec4(lineColor, 1.0);

}