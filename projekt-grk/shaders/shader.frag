#version 430 core

float ambient = 0.1;

uniform sampler2D colorTexture;
uniform sampler2D normalSampler;

in vec3 viewDirTS;
in vec3 firstLightDirTS;
in vec3 secondLightDirTS;
in float firstLightDist;
in float secondLightDist;
in vec2 vecTex;

out vec4 outColor;

void main() {

	vec3 firstL = normalize(firstLightDirTS);
	vec3 secondL = normalize(secondLightDirTS);
	vec3 V = normalize(viewDirTS);

	vec3 N = normalize(texture2D(normalSampler, vecTex).xyz * 2.0f - 1.0f);
	vec3 textureColor = texture2D(colorTexture, vecTex).xyz;

	vec3 texture = mix(textureColor, N, 0.1f);
	texture = vec3(1.0f) - exp(-texture * 1.5f);

	float firstDiffuse = max(0.0f, dot(N, firstL));
	vec3 firstR = reflect(-firstL, N);
	float firstSpecular = pow(max(dot(V, firstR), 0.0f), 16);

	float secondDiffuse = max(0.0f, dot(N, secondL));
	vec3 secondR = reflect(-secondL, N);
	float secondSpecular = pow(max(dot(V, secondR), 0.0f), 16);

	outColor = vec4(texture/(0.5f*firstLightDist) * min(1.0f, (ambient + firstDiffuse + firstSpecular))
				+ (texture/(0.5*secondLightDist) * min(1.0f, (ambient + secondDiffuse + secondSpecular))), 1.0f);
}