#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Camera {

    public:
        glm::vec2 baseView;

        Camera();
        
        void update();
        void setRayObject(glm::vec2 pos);
        void process_mouse_move(double x, double y);
		void process_mouse_buttons(int button, int action, int mods, GLFWwindow* win);


};