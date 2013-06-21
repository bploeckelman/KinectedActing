#version 330

in vec3 vert;
out vec4 outColor;

void main()
{
    outColor = vec4(vert + 0.5,1);
}
