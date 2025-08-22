#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <iostream>
#include <vector>
#include <stdio.h>

// For WASD movement
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};


class Camera {
	private:
		glm::vec3 baseView;   // orbit center
		float radius;          // distance from center
		float theta, phi;      // spherical angles
		float drag_speed;

		bool dragging, moving;
		double last_x, last_y;

		glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);


	public:

		glm::mat4 projection;

		Camera();

		glm::mat4 get_view_matrix();
		glm::vec3 get_camera_position();
		void update();
		void process_mouse_move(double x, double y);
		void process_mouse_buttons(int button, int action, int mods, GLFWwindow* win);
		void processKeyboard(Camera_Movement direction);
};

