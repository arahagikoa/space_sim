#include "camera.h"



Camera::Camera() {
	base_view = glm::vec3(0.0f, 0.0f, 0.0f);
	dragging = false;
	moving = false;

	phi = 0.0;
	theta = 3.14 / 2.0; // max theta is pi

	drag_spped = 0.1;

	last_x = base_view.x;
	last_y = base_view.y;

}

void Camera::update() {
	base_view = glm::vec3(0.0f, 0.0f, 0.0f);
	if (dragging) {
		moving = true;
	}
	else {
		moving = false;
	}
}

glm::vec3 Camera::get_camera_position() {

	float clampedElevation = glm::clamp(theta, 0.01, 3.14 - 0.01);
	// Orbit around (0,0,0) always

	glm::vec3 position = glm::vec3(
		radius * sin(clampedElevation) * cos(theta),
		radius * cos(clampedElevation),
		radius * sin(clampedElevation) * sin(theta));
	
	//std::cout << "Position: " << position.x << std::endl;

	return position;
}


void Camera::process_mouse_move(double x, double y) {

	float dx = x - last_x;
	float dy = y - last_y;

	if (dragging) {
		phi += dx * drag_spped;
		theta -= dy * drag_spped;

		theta = glm::clamp(theta, 0.01, 3.14 - 0.01); // dont fly too high Icarus
	}

	last_x = x;
	last_y = y;
	update();
}


void Camera::process_mouse_buttons(int button, int action, int mods, GLFWwindow* win) {
	if (button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_MIDDLE) {
		if (action == GLFW_PRESS) {
			dragging = true;
			glfwGetCursorPos(win, &last_x, &last_y);
		}

		else if (action == GLFW_RELEASE) {
			dragging = false;
		}
	}


}