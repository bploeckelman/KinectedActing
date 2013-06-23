#version 330

uniform mat4 camera;
uniform mat4 model;

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 texcoord;

out vec3 fragVertex;
out vec2 fragTexCoord;

void main()
{
	fragVertex = vertex;
	fragTexCoord = texcoord;
	gl_Position = camera * model * vec4(vertex, 1);
}
