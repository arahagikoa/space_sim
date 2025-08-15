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

	public:
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

};

