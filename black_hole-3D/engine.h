#pragma once
#ifndef ENGINE_H
#define ENGINE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stb_image.h>

class Engine {

public:
	int WIDTH;
	int HEIGHT;

	GLuint backgroundShader;
	GLuint backgroundVAO, backgroundVBO;
	GLuint backgroundTexture;

	GLFWwindow* window;
	GLuint shaderProgram;

	std::string fragmentShaderSourceFile = "./shaders/bh.frag";
	std::string vertexShaderSourceFile = "./shaders/bh.vert";

	Engine(int w, int h) : WIDTH(w), HEIGHT(h), window(nullptr),
		shaderProgram(0), backgroundShader(0),
		backgroundVAO(0), backgroundVBO(0), backgroundTexture(0) {}

	bool init();
	void run();
	void processInput();
	void render();
	void cleanup();

	std::string loadShaderFile(const std::string& shaderSource);
	GLuint CreateShaderProgram();
	GLuint CreateShaderProgram(const char* vertexPath, const char* fragmentPath);

	void loadBackground(const char* imagePath, const char* vertPath, const char* fragPath);
	void drawBackground();


};

#endif // !ENGINE_H