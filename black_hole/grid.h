#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>
#include <iostream>
#include <vector>



class Grid {
	public:
		float WIDTH;

		GLuint GridVBO, GridVAO;

		int vertexCount;

		Grid(float width);
		void setupGrid(float WIDTH, float HEIGHT);
		void drawGrid(GLuint shaderProgram);


};
