#pragma once
#ifndef RAY_H
#define RAY_H


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>
#include <iostream>
#include <vector>

#ifndef C_VEL
#define C_VEL 299792458.0
#endif


class Ray {

	public:
		glm::vec2 dir;
		double x;
		double y;
		std::vector<glm::vec2> trail;

		GLuint VAO = 0;
		GLuint VBO = 0;

		Ray(glm::vec2 pos, glm::vec2 dir);
		void draw_ray(GLuint shaderProgram);
		void step();


};




#endif