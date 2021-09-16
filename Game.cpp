#include"Game.h"

Game::Game(unsigned int _winWidth, unsigned int _winHeight, unsigned int _samples)
{
	winWidth = _winWidth;
	winHeight = _winHeight;
	samples = _samples;
}


void Game::run()
{
	srand((unsigned int)time(NULL)); // random number seed 

	/* --------------------- window stuff --------------------- */
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, samples);

	GLFWwindow* window = glfwCreateWindow(winWidth, winHeight, "3D Game", NULL, NULL);
	glfwSetWindowPos(window, (1920 / 2) - (winWidth / 2), 30);
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, winWidth, winWidth);


	/* --------------------- stuff for blocks --------------------- */
	Terrain world;

	Shader shaderProgram("default.vert", "default.frag");

	VAO VAO1;
	VAO1.Bind();
	VBO VBO1(world.vertices.data(), sizeof(float) * world.vertices.size());
	//EBO EBO1(cubeIndices, sizeof(cubeIndices));

	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 9 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 3, 1, GL_FLOAT, 9 * sizeof(float), (void*)(8 * sizeof(float)));

	VAO1.Unbind();
	VBO1.Unbind();
	//EBO1.Unbind();

	Texture texture("dirt_block/snow.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	Texture texture2("dirt_block/top.png", GL_TEXTURE_2D, GL_TEXTURE1, GL_RGB, GL_UNSIGNED_BYTE);
	int textures[] = { 0, 1 };

	shaderProgram.Activate();
	shaderProgram.setList("tex_array", 2, textures);

	/* --------------------- other stuff --------------------- */
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	Camera camera(winWidth, winHeight, glm::vec3(0.0f, 0.0f, 2.0f), 1.25f, 100.0f);

	/* --------------------- skybox stuff --------------------- */
	Shader skyboxShader("skybox.vert", "skybox.frag");

	VAO skyboxVAO;
	skyboxVAO.Bind();
	VBO skyboxVBO(skyboxVertices, sizeof(skyboxVertices));
	EBO skyboxEBO(skyboxIndices, sizeof(skyboxIndices));

	skyboxVAO.LinkAttrib(skyboxVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

	skyboxVAO.Unbind();
	skyboxVBO.Unbind();
	skyboxEBO.Unbind();

	std::string facesCubemap[6] =
	{
		"skybox/right.png",
		"skybox/left.png",
		"skybox/top.png",
		"skybox/bottom.png",
		"skybox/front.png",
		"skybox/back.png"
	};

	unsigned int cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);

		stbi_set_flip_vertically_on_load(false);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}

	skyboxShader.Activate();
	skyboxShader.setInt("skybox", 0);

	loop(window, shaderProgram, skyboxShader,
		VAO1, skyboxVAO, camera, world, 
		texture, texture2, cubemapTexture);
}

void Game::loop(
	GLFWwindow* window, 
	Shader shaderProgram, Shader skyboxShader,
	VAO VAO1, VAO skyboxVAO,
	Camera camera, Terrain world, 
	Texture texture, Texture texture2, 
	unsigned int cubemapTexture)
{
	double previousTime = glfwGetTime();
	int fps = 0;
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// --- counting frames
		double currentTime = glfwGetTime();
		fps++;
		if (currentTime - previousTime >= 1.0)
		{
			std::stringstream ss;
			ss << "3D Game" << " [" << fps << " FPS]";
			glfwSetWindowTitle(window, ss.str().c_str());
			fps = 0;
			previousTime = currentTime;
		}

		// ----- draw sequence ----- //
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// player camera movement
		shaderProgram.Activate();
		camera.Move(window);
		camera.Matrix(fov, 0.1f, render_distance, shaderProgram, "camMatrix");

		// cubes
		texture.Bind();
		texture2.Bind();
		VAO1.Bind();

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(world.offset[0], world.offset[1], world.offset[2]));
		shaderProgram.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(sizeof(float) * world.vertices.size()));


		drawSkybox(skyboxShader, camera, cubemapTexture, skyboxVAO);

		// update screen
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Game::destroy(VAO VAO1, VBO VBO1, VAO skyboxVAO, VBO skyboxVBO, EBO skyboxEBO,
	Texture texture, Texture texture2,
	Shader shaderProgram, Shader skyboxShader,
	GLFWwindow* window)
{
	VAO1.Delete();
	VBO1.Delete();
	//EBO1.Delete();
	texture.Delete();
	texture2.Delete();
	shaderProgram.Delete();

	skyboxVAO.Delete();
	skyboxVBO.Delete();
	skyboxEBO.Delete();
	skyboxShader.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
}


void Game::processInput(GLFWwindow* _window)
{
	if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS) {
		fov += 5.0f;
		std::cout << fov << std::endl;
	}
	if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		fov -= 5.0f;
		std::cout << fov << std::endl;
	}
}

void Game::drawSkybox(Shader _skyboxShader, Camera _camera, unsigned int _cubemapTexture, VAO _skyboxVAO) {
	glDepthFunc(GL_LEQUAL);
	_skyboxShader.Activate();
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	view = glm::mat4(glm::mat3(glm::lookAt(_camera.Position, _camera.Position + _camera.Orientation, _camera.Up)));
	projection = glm::perspective(glm::radians(45.0f), (float)(winWidth / winHeight), 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(_skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(_skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _cubemapTexture);

	_skyboxVAO.Bind();
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glDepthFunc(GL_LESS);
}