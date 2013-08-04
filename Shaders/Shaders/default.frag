#version 330

uniform mat4 model;

uniform sampler2D tex;
uniform vec2 texscale;
uniform int useLighting;

uniform struct Light {
	vec3 position;
	vec3 intensities;
	float attenuation;
	float ambientCoefficient;
} light;

in vec3 fragVertex;
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec4 fragColor;

out vec4 finalColor;

void main()
{
	if (useLighting == 0) {
		finalColor = fragColor * texture(tex, texscale * fragTexCoord);
		return;
	}

	vec3 normal = normalize(transpose(inverse(mat3(model))) * fragNormal);
	vec3 surfacePos = vec3(model * vec4(fragVertex, 1));
	vec4 surfaceColor = texture(tex, texscale * fragTexCoord);
	vec3 surfaceToLight = normalize(light.position - surfacePos);

	vec3 ambient = light.ambientCoefficient * surfaceColor.rgb * light.intensities * fragColor;

	float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
	vec3 diffuse = diffuseCoefficient * surfaceColor.rgb * light.intensities;

	float distanceToLight = length(light.position - surfacePos);
	float attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2));

	vec3 linearColor = ambient + attenuation * diffuse;

	vec3 gamma = vec3(1.0 / 2.2);

	float alpha = max(0.0, min(fragColor.a, surfaceColor.a));
	finalColor = vec4(pow(linearColor, gamma), alpha);
}
