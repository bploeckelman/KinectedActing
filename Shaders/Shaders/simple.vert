#version 330

uniform mat4 camera;
uniform mat4 model;

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec4 color;

out vec3 fragVertex;
out vec4 fragColor;

void main()
{
	fragVertex = vertex;
	fragColor = color;
	gl_Position = camera * model * vec4(vertex, 1);
}
