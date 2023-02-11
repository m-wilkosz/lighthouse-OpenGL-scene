#include "glew.h"
#include "glm.hpp"
#include "ext.hpp"
#include <GLFW/glfw3.h>
#include <gtx/transform.hpp>
#include <gtx/euler_angles.hpp>

#include <iostream>
#include <string>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Texture.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "SOIL/stb_image_aug.h"
#include "SOIL/SOIL.h"

namespace texture {
	GLuint moon;
	GLuint moonNormal;

	GLuint water;
	GLuint waterNormal;

	GLuint rocks;
	GLuint rocksNormal;

	GLuint tower;
	GLuint towerNormal;
	GLuint towerTop;
	GLuint towerTopNormal;
	GLuint lamp;
	GLuint lampNormal;
	GLuint roof;
	GLuint roofNormal;

	GLuint grass;
	GLuint grassNormal;
	GLuint trunk;
	GLuint trunkNormal;
	GLuint tree;
	GLuint treeNormal;

	GLuint shipBottom;
	GLuint shipBottomNormal;
	GLuint shipMiddle;
	GLuint shipMiddleNormal;
	GLuint shipTop;
	GLuint shipTopNormal;
}
GLuint cubeMapTextureID;

GLuint program;
GLuint programMoon;
GLuint programLight;
GLuint programWater;
GLuint programSkyBox;

Core::Shader_Loader shaderLoader;

Core::RenderContext cubeContext;
Core::RenderContext sphereContext;
Core::RenderContext surfaceContext;
Core::RenderContext rocksContext;
Core::RenderContext towerContext;
Core::RenderContext towerTopContext;
Core::RenderContext lampContext;
Core::RenderContext lightContext;
Core::RenderContext towerRoofContext;
Core::RenderContext groundContext;
Core::RenderContext trunkContext;
Core::RenderContext treeContext;
Core::RenderContext shipBottomContext;
Core::RenderContext shipMiddleContext;
Core::RenderContext shipTopContext;

glm::vec3 cameraPos = glm::vec3(20.0f, 8.0f, 0.0f);
glm::vec3 cameraDir = glm::vec3(1.0f, 0.0f, 0.0f);

glm::vec3 shipPos = glm::vec3(16.0f, 0.0f, 0.0f);
glm::vec3 shipDir = glm::vec3(-1.0f, 0.0f, 0.0f);

float aspectRatio = 1.0f;

glm::mat4 lightMatrix;

glm::mat4 createCameraMatrix() {

	glm::vec3 cameraSide = glm::normalize(glm::cross(cameraDir,glm::vec3(0.0f,1.0f,0.0f)));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraSide,cameraDir));
	glm::mat4 cameraRotrationMatrix = glm::mat4({
		cameraSide.x,cameraSide.y,cameraSide.z,0.0f,
		cameraUp.x,cameraUp.y,cameraUp.z,0.0f,
		-cameraDir.x,-cameraDir.y,-cameraDir.z,0.0f,
		0.0f,0.0f,0.0f,1.0f,
		});
	cameraRotrationMatrix = glm::transpose(cameraRotrationMatrix);
	glm::mat4 cameraMatrix = cameraRotrationMatrix * glm::translate(-cameraPos);

	return cameraMatrix;
}

glm::mat4 createPerspectiveMatrix() {

	glm::mat4 perspectiveMatrix;
	float n = 0.1f;
	float f = 30.0f;
	perspectiveMatrix = glm::mat4({
		1.0f,0.0f,0.0f,0.0f,
		0.0f,aspectRatio,0.0f,0.0f,
		0.0f,0.0f,(f+n)/(n-f),2*f*n/(n-f),
		0.0f,0.0f,-1.0f,0.0f,
		});
	perspectiveMatrix = glm::transpose(perspectiveMatrix);

	return perspectiveMatrix;
}

void drawObjectColor(Core::RenderContext &context, glm::mat4 modelMatrix, glm::vec3 color, GLuint program) {

	glUseProgram(program);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "lightMatrix"), 1, GL_FALSE, (float*)&lightMatrix);
	glUniform3f(glGetUniformLocation(program, "color"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "firstLightPos"), 1.0f, 10.0f, 0.0f);
	Core::DrawContext(context);
}

void drawObjectTexture(Core::RenderContext &context, glm::mat4 modelMatrix, GLuint textureID, GLuint normalmapID, GLuint program) {

	glUseProgram(program);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "lightMatrix"), 1, GL_FALSE, (float*)&lightMatrix);
	glUniform3f(glGetUniformLocation(program, "firstLightPos"), 1.0f, 10.0f, 0.0f);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	Core::SetActiveTexture(textureID, "colorTexture", program, 0);
	Core::SetActiveTexture(normalmapID, "normalSampler", program, 1);
	Core::DrawContext(context);
}

void loadCubeMap(char *filePaths[6]) {

	glGenTextures(1, &cubeMapTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);

	int w, h;
	for (unsigned int i = 0; i < 6; i++) {

		unsigned char* image = SOIL_load_image(filePaths[i], &w, &h, 0, SOIL_LOAD_RGBA);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image
		);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void renderScene(GLFWwindow *window) {

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float time = glfwGetTime();

	lightMatrix = glm::translate(glm::vec3(9.45f, 5.35f, 7.15f)) * glm::eulerAngleY(1.57f * sin(time/3) / 2.0f - 1.8f);

	glm::vec3 shipSide = glm::normalize(glm::cross(shipDir, glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::vec3 shipUp = glm::normalize(glm::cross(shipSide, shipDir));
	glm::mat4 shipCameraRotrationMatrix = glm::mat4({
		shipSide.x,shipSide.y,shipSide.z,0.0f,
		shipUp.x,shipUp.y,shipUp.z,0.0f,
		-shipDir.x,-shipDir.y,-shipDir.z,0.0f,
		0.0f,0.0f,0.0f,1.0f,
		});

	glDisable(GL_DEPTH_TEST);
	glUseProgram(programSkyBox);
	glUniformMatrix4fv(glGetUniformLocation(programSkyBox, "transformation"), 1, GL_FALSE,
		(float*)&(createPerspectiveMatrix() * createCameraMatrix() * glm::translate(shipPos)));
	Core::SetActiveTexture(cubeMapTextureID, "skybox", programSkyBox, 0);
	Core::DrawContext(cubeContext);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(programWater);
	glUniform1f(glGetUniformLocation(programWater, "move"), sin(time)/1000);
	drawObjectTexture(surfaceContext, glm::translate(glm::vec3(0.0f, -0.15f, 0.0f)) * glm::translate(shipPos), texture::water, texture::waterNormal, programWater);

	drawObjectTexture(rocksContext, glm::translate(glm::vec3(10.0f, 0.0f, 4.0f)), texture::rocks, texture::rocksNormal, program);

	drawObjectTexture(towerContext, glm::translate(glm::vec3(9.5f, 2.0f, 7.0f)), texture::tower, texture::towerNormal, program);
	drawObjectTexture(towerTopContext, glm::translate(glm::vec3(9.5f, 5.0f, 7.0f)), texture::towerTop, texture::towerTopNormal, program);
	drawObjectTexture(lampContext,
		lightMatrix,
		texture::lamp,
		texture::lampNormal,
		program);
	drawObjectTexture(towerRoofContext, glm::translate(glm::vec3(9.5f, 6.0f, 7.0f)), texture::roof, texture::roofNormal, program);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	drawObjectColor(lightContext, lightMatrix, glm::vec3(1.0f, 1.0f, 1.0f), programLight);

	drawObjectTexture(groundContext, glm::translate(glm::vec3(-35.0f, -0.75f, -6.0f)) * glm::scale(glm::vec3(0.08f)), texture::grass, texture::grassNormal, program);

	drawObjectTexture(sphereContext, glm::translate(glm::vec3(1.0f, 10.0f, 0.0f)) * glm::scale(glm::vec3(0.8f)), texture::moon, texture::moonNormal, programMoon);

	for (int i = 0; i < 22; i++) {
		for (int j = abs(i % 2 - 1); j < 44; j += 2) {

			drawObjectTexture(trunkContext, glm::translate(glm::vec3(2.0f - (float)j, 1.2f, -6.0f - (float)i)), texture::trunk, texture::trunkNormal, program);
			drawObjectTexture(treeContext, glm::translate(glm::vec3(2.0f - (float)j, 1.2f, -6.0f - (float)i)), texture::tree, texture::treeNormal, program);
		}
	}

	drawObjectTexture(shipBottomContext,
		glm::translate(shipPos) * shipCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()),
		texture::shipBottom,
		texture::shipBottomNormal,
		program);
	drawObjectTexture(shipMiddleContext,
		glm::translate(shipPos) * shipCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()),
		texture::shipMiddle,
		texture::shipMiddleNormal,
		program);
	drawObjectTexture(shipTopContext,
		glm::translate(shipPos) * shipCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()),
		texture::shipTop,
		texture::shipTopNormal,
		program);

	drawObjectTexture(shipBottomContext,
		glm::rotate(glm::translate(glm::vec3(19.0f - time/3, 0.0f, 0.0f)), -1.57f, glm::vec3(0.0f, 1.0f, 0.0f)),
		texture::shipBottom,
		texture::shipBottomNormal,
		program);
	drawObjectTexture(shipMiddleContext,
		glm::rotate(glm::translate(glm::vec3(19.0f - time/3, 0.0f, 0.0f)), -1.57f, glm::vec3(0.0f, 1.0f, 0.0f)),
		texture::shipMiddle,
		texture::shipMiddleNormal,
		program);
	drawObjectTexture(shipTopContext,
		glm::rotate(glm::translate(glm::vec3(19.0f - time/3, 0.0f, 0.0f)), -1.57f, glm::vec3(0.0f, 1.0f, 0.0f)),
		texture::shipTop,
		texture::shipTopNormal,
		program);

	glUseProgram(0);
	glfwSwapBuffers(window);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {

	aspectRatio = width / float(height);
	glViewport(0, 0, width, height);
}

void loadModelToContext(std::string path, Core::RenderContext &context) {

	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {

		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	context.initFromAssimpMesh(scene->mMeshes[0]);
}

void init(GLFWwindow *window) {

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);
	program = shaderLoader.CreateProgram("shaders/shader.vert", "shaders/shader.frag");
	programMoon = shaderLoader.CreateProgram("shaders/shader_moon.vert", "shaders/shader_moon.frag");
	programLight = shaderLoader.CreateProgram("shaders/shader_light.vert", "shaders/shader_light.frag");
	programWater = shaderLoader.CreateProgram("shaders/shader_water.vert", "shaders/shader_water.frag");
	programSkyBox = shaderLoader.CreateProgram("shaders/shader_skybox.vert", "shaders/shader_skybox.frag");

	loadModelToContext("./models/cube.obj", cubeContext);
	loadModelToContext("./models/sphere.obj", sphereContext);
	loadModelToContext("./models/surface.obj", surfaceContext);
	loadModelToContext("./models/rocks.obj", rocksContext);
	loadModelToContext("./models/tower.obj", towerContext);
	loadModelToContext("./models/towerTop.obj", towerTopContext);
	loadModelToContext("./models/lamp.obj", lampContext);
	loadModelToContext("./models/light.obj", lightContext);
	loadModelToContext("./models/towerRoof.obj", towerRoofContext);
	loadModelToContext("./models/ground.obj", groundContext);
	loadModelToContext("./models/trunk.obj", trunkContext);
	loadModelToContext("./models/tree.obj", treeContext);
	loadModelToContext("./models/shipBottom.obj", shipBottomContext);
	loadModelToContext("./models/shipMiddle.obj", shipMiddleContext);
	loadModelToContext("./models/shipTop.obj", shipTopContext);

	texture::moon = Core::LoadTexture("textures/moon.jpg");
	texture::moonNormal = Core::LoadTexture("textures/moon_normal.jpg");
	texture::water = Core::LoadTexture("textures/water.jpg");
	texture::waterNormal = Core::LoadTexture("textures/water_normal.jpg");
	texture::rocks = Core::LoadTexture("textures/rocks.png");
	texture::rocksNormal = Core::LoadTexture("textures/rocks_normal.png");
	texture::tower = Core::LoadTexture("textures/tower.jpg");
	texture::towerNormal = Core::LoadTexture("textures/tower_normal.jpg");
	texture::towerTop = Core::LoadTexture("textures/towerTop.jpg");
	texture::towerTopNormal = Core::LoadTexture("textures/towerTop_normal.jpg");
	texture::lamp = Core::LoadTexture("textures/lamp.jpg");
	texture::lampNormal = Core::LoadTexture("textures/lamp_normal.jpg");
	texture::roof = Core::LoadTexture("textures/roof.jpg");
	texture::roofNormal = Core::LoadTexture("textures/roof_normal.jpg");
	texture::grass = Core::LoadTexture("textures/grass.jpg");
	texture::grassNormal = Core::LoadTexture("textures/grass_normal.jpg");
	texture::trunk = Core::LoadTexture("textures/trunk.jpg");
	texture::trunkNormal = Core::LoadTexture("textures/trunk_normal.jpg");
	texture::tree = Core::LoadTexture("textures/tree.jpg");
	texture::treeNormal = Core::LoadTexture("textures/tree_normal.jpg");
	texture::shipBottom = Core::LoadTexture("textures/shipBottom.jpg");
	texture::shipBottomNormal = Core::LoadTexture("textures/shipBottom_normal.jpg");
	texture::shipMiddle = Core::LoadTexture("textures/shipMiddle.jpg");
	texture::shipMiddleNormal = Core::LoadTexture("textures/shipMiddle_normal.jpg");
	texture::shipTop = Core::LoadTexture("textures/shipTop.jpg");
	texture::shipTopNormal = Core::LoadTexture("textures/shipTop_normal.jpg");

	char *filePaths[6] = { "textures/skybox/sky_rt.jpg", "textures/skybox/sky_lf.jpg", "textures/skybox/sky_up.jpg",
							"textures/skybox/sky_dn.jpg", "textures/skybox/sky_bk.jpg", "textures/skybox/sky_ft.jpg" };
	loadCubeMap(filePaths);
}

void shutdown(GLFWwindow *window) {

	shaderLoader.DeleteProgram(program);
}

void processInput(GLFWwindow *window) {

	float angleSpeed = 0.05f;
	float moveSpeed = 0.025f;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		shipPos += shipDir * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		shipPos -= shipDir * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		shipDir = glm::vec3(glm::eulerAngleY(angleSpeed) * glm::vec4(shipDir, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		shipDir = glm::vec3(glm::eulerAngleY(-angleSpeed) * glm::vec4(shipDir, 0.0f));

	cameraPos = shipPos - 5.0f * shipDir + glm::vec3(0.0f, 3.5f, 0.0f);
	cameraDir = shipDir;
}

void renderLoop(GLFWwindow *window) {

	while (!glfwWindowShouldClose(window)) {

		processInput(window);
		renderScene(window);
		glfwPollEvents();
	}
}
