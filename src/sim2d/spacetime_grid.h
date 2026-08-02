#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>
#include <iostream>
#include <vector>



class SpacetimeGrid {
	public:
		int WIDTH;

		GLuint GridVBO, GridVAO;

		int vertexCount;

		SpacetimeGrid(int width);
		void setupGrid(int WIDTH, int HEIGHT);
		void drawGrid(GLuint shaderProgram);


};
