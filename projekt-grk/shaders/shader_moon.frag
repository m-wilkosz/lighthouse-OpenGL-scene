#version 430 core

uniform sampler2D colorTexture;
uniform sampler2D normalSampler;

in vec2 vTex;

out vec4 outColor;

void main() {

	vec3 N = normalize(texture2D(normalSampler, vTex).xyz * 2.0f - 1.0f);
	vec3 textureColor = texture2D(colorTexture, vTex).xyz;

	vec3 texture = mix(textureColor, N, 0.1f);

	outColor = vec4(texture, 1.0f);
}