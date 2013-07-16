#version 330

uniform vec4 color;

in vec3 fragVertex;
in vec4 fragColor;

out vec4 finalColor;

void main()
{
	finalColor = color;
}
