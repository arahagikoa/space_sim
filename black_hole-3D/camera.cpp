#include "camera.h"



Camera::Camera() {
	baseView = glm::vec3(0.0f, 0.0f, 0.0f);

	dragging = false;
	moving = false;

	phi = 0.0;
	theta = 3.14 / 2.0; // max theta is pi

	drag_speed = 0.01f;
	radius = 100.0f; // works for e27 kg mass

	last_x = baseView.x;
	last_y = baseView.y;


	projection = glm::perspective(
		glm::radians(45.0f),
		(float)600.0 / (float)600.0,
		0.1f,
		1e25f
	);
}


glm::mat4 Camera::get_view_matrix() {
	cameraPos = this->get_camera_position();
	
	glm::mat4 view;
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	return view;
}


glm::vec3 Camera::get_camera_position() {
	float clampedElevation = glm::clamp((double)theta, 0.01, 3.14 - 0.01);

	glm::vec3 position = glm::vec3(
		radius * sin(clampedElevation) * cos(phi),
		radius * cos(clampedElevation),
		radius * sin(clampedElevation) * sin(phi));

	return position;
}


void Camera::update() {
	cameraPos = baseView;

	if (dragging) {
		moving = true;
	}
	else {
		moving = false;
	}
}


void Camera::process_mouse_move(double x, double y) {

	float dx = x - last_x;
	float dy = y - last_y;

	if (dragging) {
		phi -= dx * drag_speed;
		theta += dy * drag_speed;

		theta = glm::clamp((double)theta, 0.01, 3.14 - 0.01); // dont fly too high Icarus
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

void Camera::processKeyboard(Camera_Movement direction)
{
	float velocity = 0.1f;



}