#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>
#include <iostream>
#include <vector>
#include <stdio.h>

class Camera {
	glm::vec3 base_view;

	glm::vec3 f;
	glm::vec3 r;
	glm::vec3 u;

	public:
		glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);

		double phi;
		double theta;

		float drag_spped;

		double last_x; double last_y;
		bool dragging;
		bool moving;

		double radius = 100;
		Camera();
		glm::vec3 get_camera_position();
		void update();
		void process_mouse_move(double x, double y);
		void process_mouse_buttons(int button, int action, int mods, GLFWwindow* win);
		void process_scroll(GLFWwindow* win, double xoffset, double yoffset);
		// Proces UV
		glm::vec3 get_forward();
		glm::vec3 get_right();
		glm::vec3 get_up();


};

