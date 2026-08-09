#include "camera.h"



Camera::Camera() {
	base_view = glm::vec3(0.0f, 0.0f, 0.0f);
	dragging = false;
	moving = false;

	phi = 0.0;
	theta = 3.14 / 2.0; // max theta is pi

	drag_spped = 0.001;

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
		radius * sin(clampedElevation) * cos(phi),
		radius * cos(clampedElevation),
		radius * sin(clampedElevation) * sin(phi));

	return position;
}


void Camera::process_mouse_move(double x, double y) {

	float dx = x - last_x;
	float dy = y - last_y;

	if (dragging) {
		phi -= dx * drag_spped;
		theta += dy * drag_spped;

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

void Camera::process_scroll(GLFWwindow* win, double xoffset, double yoffset) {
	// Trackpads send horizontal-only events; taking the sign by dividing by a
	// zero magnitude would put a NaN into radius and poison every ray.
	if (yoffset == 0.0) return;

	// Zoom is perceptually a ratio, so scale the current radius. Multiplying by
	// a signed +/-1.1 instead sends the radius negative on one scroll direction,
	// where the clamp then pins it to the lower bound.
	const double step = 1.1;
	this->radius *= (yoffset > 0.0) ? (1.0 / step) : step;

	// Lower bound sits just outside the photon sphere at 1.5 r_s.
	this->radius = glm::clamp(this->radius, 2.0, 200.0);
	//std::cout << "RADIUS: " << radius << std::endl;
}

glm::vec3 Camera::get_forward() {
	glm::vec3 camPos = get_camera_position();
	this->f = glm::normalize(glm::vec3(-camPos.x, -camPos.y, -camPos.z));
	return f;
}
glm::vec3 Camera::get_right() {
	this->r = glm::normalize(glm::cross(f, world_up));
	return r;
}
glm::vec3 Camera::get_up() {
	//if (this->f == nullptr) {
	//	get_forward();
	//}
		
	//if (this->r == nullptr) {
	//	get_forward();
	//}
	this->u = glm::cross(this->r, this->f);
	return u;
}