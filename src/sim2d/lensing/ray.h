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
		double r, phi;
		double dr, dphi;
		double d2r, d2phi;
		double E, L;

		std::vector<glm::vec2> trail;

		GLuint VAO_point, VBO_point;
		GLuint VAO_trail, VBO_trail;

		Ray(glm::vec2 pos, glm::vec2 dir, double r_s);
		void draw_ray(GLuint shaderProgram);
		void step(double r_s, double dlambda);


};




#endif