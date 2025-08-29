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
	double x, y, z;

	double r, phi, theta;
	double dr, dphi, dtheta;
	double d2r, d2phi, d2theta;
	double E, L;

	std::vector<glm::vec3> trail;

	GLuint VAO_point, VBO_point;
	GLuint VAO_trail, VBO_trail;

	Ray(glm::vec3 pos, glm::vec3 dir, double r_s);
	void draw_ray(GLuint shaderProgram);
	void step(double r_s, double dlambda);


};




#endif