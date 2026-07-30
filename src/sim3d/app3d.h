#pragma once

#ifndef ENGINE_H
#define ENGINE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>


class Engine {

public:
	int WIDTH;
	int HEIGHT;

	GLFWwindow* window;
	GLuint shaderProgram;

	std::string fragmentShaderSourceFile = "./shaders/bh.frag";
	std::string vertexShaderSourceFile = "./shaders/bh.vert";

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
	std::string laodShaderFile(const std::string& shaderSource);
	GLuint createShaderProgram();

};
#endif // !ENGINE_H