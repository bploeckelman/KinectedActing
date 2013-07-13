#version 330

uniform vec4 color;

in vec3 fragVertex;

out vec4 finalColor;

void main()
{
	finalColor = color;
}
