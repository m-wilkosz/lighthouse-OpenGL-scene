#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

uniform mat4 transformation;
uniform mat4 modelMatrix;
uniform mat4 lightMatrix;
uniform vec3 firstLightPos;
uniform vec3 cameraPos;

out vec3 viewDirTS;
out vec3 firstLightDirTS;
out vec3 secondLightDirTS;
out float firstLightDist;
out float secondLightDist;
out vec2 vecTex;

void main() {

	gl_Position = transformation * vec4(vertexPosition, 1.0f);
	vec3 worldPos = (modelMatrix * vec4(vertexPosition, 1.0f)).xyz;

	vec3 vecNormal = normalize(modelMatrix * vec4(vertexNormal, 0.0f)).xyz;
	vec3 w_tangent = normalize(modelMatrix * vec4(vertexTangent, 0.0f)).xyz;
	vec3 w_bitangent = normalize(modelMatrix * vec4(vertexBitangent, 0.0f)).xyz;

	mat3 TBN = transpose(mat3(w_tangent, w_bitangent, vecNormal));

	vec3 V = normalize(cameraPos-worldPos);
	viewDirTS = TBN * V;

	firstLightDist = distance(worldPos, firstLightPos);
	vec3 firstL = normalize(firstLightPos-worldPos);
	firstLightDirTS = TBN * firstL;

	vec3 secondLightPos = (lightMatrix * vec4(-2.0f, 0.0f, 1.0f, 1.0f)).xyz;
	secondLightDist = distance(worldPos, secondLightPos);
	vec3 secondL = normalize(secondLightPos-worldPos);
	secondLightDirTS = TBN * secondL;

	vecTex = vertexTexCoord;
	vecTex.y = 1.0 - vecTex.y;
}