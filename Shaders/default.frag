#version 330

uniform sampler2D tex;
uniform vec2 texscale;

in vec3 fragVertex;
in vec2 fragTexCoord;
in vec3 fragNormal;

out vec4 finalColor;

void main()
{
	finalColor = texture(tex, texscale * fragTexCoord);
}
