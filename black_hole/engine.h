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


class Engine {

	public:
		int WIDTH;
		int HEIGHT;
		
		GLFWwindow* window;
		GLuint shaderProgram;

		std::string fragmentShaderSourceFile = "E:/Projekty/black_hole/black_hole/shaders/frag.frag"; 
		std::string vertexShaderSourceFile = "E:/Projekty/black_hole/black_hole/shaders/vertex.vert";

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
		
		std::string loadShaderFile(const std::string& shaderSource);
		GLuint CreateShaderProgram();


};

#endif // !ENGINE_H
