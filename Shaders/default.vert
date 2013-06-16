#version 330

uniform mat4 modelview_mat;
uniform mat4 projection_mat;

layout(location = 0) in vec4 position;

void main()
{
	gl_Position = projection_mat * modelview_mat * position;
}
