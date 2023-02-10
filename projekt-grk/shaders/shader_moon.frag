#version 430 core

uniform sampler2D colorTexture;
uniform sampler2D normalSampler;

in vec2 vTex;

out vec4 outColor;

void main() {

	vec3 texture = texture2D(colorTexture, vTex).xyz;

	outColor = vec4(texture, 1.0f);
}
