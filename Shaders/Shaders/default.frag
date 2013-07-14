#version 330

uniform sampler2D tex;
uniform vec2 texscale;
uniform vec3 light;

in vec3 fragVertex;
in vec2 fragTexCoord;
in vec3 fragNormal;

out vec4 finalColor;

void main()
{
	vec3 lightDir = normalize(light);
	float d = max(0.0, dot(fragNormal, lightDir));
	finalColor = d * texture(tex, texscale * fragTexCoord);
}
