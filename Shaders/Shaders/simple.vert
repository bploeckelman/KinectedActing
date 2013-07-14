#version 330

uniform mat4 camera;
uniform mat4 model;

layout(location = 0) in vec3 vertex;

out vec3 fragVertex;

void main()
{
	fragVertex = vertex;
	gl_Position = camera * model * vec4(vertex, 1);
}
