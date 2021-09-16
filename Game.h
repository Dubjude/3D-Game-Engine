#ifndef GAME_CLASS_H
#define GAME_CLASS_H

#include<iostream>
#include<sstream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<vector>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"Texture.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Camera.h"
#include"Terrain.h"

class Game
{
public:
	Game(unsigned int _winWidth, unsigned int _winHeight, unsigned int _samples);
	void run();

private:
	void loop(
		GLFWwindow* window,
		Shader shaderProgram, Shader skyboxShader,
		VAO VAO1, VAO skyboxVAO,
		Camera camera, Terrain world,
		Texture texture, Texture texture2,
		unsigned int cubemapTexture);

	void destroy(VAO VAO1, VBO VBO1, VAO skyboxVAO, VBO skyboxVBO, EBO skyboxEBO,
		Texture texture, Texture texture2,
		Shader shaderProgram, Shader skyboxShader,
		GLFWwindow* window);

	unsigned int winWidth;
	unsigned int winHeight;
	unsigned int samples;

	float render_distance = 1000.0f;
	float fov = 45.0f;

	GLuint skyboxIndices[36] =
	{
		1, 2, 6,
		6, 5, 1,

		0, 4, 7,
		7, 3, 0,

		4, 5, 6,
		6, 7, 4,

		0, 3, 2,
		2, 1, 0,

		0, 1, 5,
		5, 4, 0,

		3, 7, 6,
		6, 2, 3
	};

	GLfloat skyboxVertices[24] =
	{
		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f
	};

	void processInput(GLFWwindow* _window);
	void drawSkybox(Shader _skyboxShader, Camera _camera, unsigned int _cubemapTexture, VAO _skyboxVAO);
};


#endif
