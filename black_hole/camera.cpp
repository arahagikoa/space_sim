#include "camera.h"



Camera::Camera(){

}
void Camera::update(){

}
void Camera::setRayObject(glm::vec2 pos){

}
void Camera::process_mouse_move(double x, double y){

}
void Camera::process_mouse_buttons(int button, int action, int mods, GLFWwindow* win) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {

			glfwGetCursorPos(win, &last_x, &last_y);
            
		}

		else if (action == GLFW_RELEASE) {
		}
	}
}
