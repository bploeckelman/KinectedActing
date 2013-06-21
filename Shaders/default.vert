#version 330

uniform mat4 camera;
uniform mat4 model;

layout(location = 0) in vec3 position;

out vec3 vert;

void main()
{
    vert = position;
    gl_Position = camera * model * vec4(position, 1);
}
