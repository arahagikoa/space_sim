#pragma once

#ifndef ENGINE_H
#define ENGINE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Engine {

public:
	int WIDTH;
	int HEIGHT;

	GLFWwindow* window;
	GLuint shaderProgram;

	Engine(int width, int height) {
		WIDTH = width;
		HEIGHT = height;
		window = nullptr;
	}

	bool init();
	void run();
	void processInput();
	void render();
	void cleanup();
	GLuint CreateShaderProgram();

};

#endif // !ENGINE_H