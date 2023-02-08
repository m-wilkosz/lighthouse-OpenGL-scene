#version 430 core

uniform vec3 color;

in vec3 vPos;

out vec4 outColor;

void main() {

	outColor = vec4(color, 1.0f + vPos.x * 0.04f);
}