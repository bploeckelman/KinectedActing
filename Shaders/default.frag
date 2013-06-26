#version 330

uniform sampler2D tex;

in vec3 fragVertex;
in vec2 fragTexCoord;

out vec4 finalColor;

void main()
{
	finalColor = texture(tex, fragTexCoord);
}
