#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <chrono>
#include <fstream>
#include <sstream>

#include "engine.h"
#include "black_hole.h"
#include "ray.h"


using Clock = std::chrono::high_resolution_clock;

int WIDTH = 1000; int HEIGHT = 900;
std::vector<Ray> rays;

int main() {
    Engine engine(WIDTH, HEIGHT);
    
    if (!engine.init()) return -1;

    engine.shaderProgram = engine.CreateShaderProgram();

    BlackHole bh(WIDTH / 2.0f, HEIGHT / 2.0f, 8.54e35);
    
    rays.push_back(Ray(glm::vec2(0.0f, HEIGHT / 2.0f), glm::vec2(1.0f, 0.0f)));

    while (!glfwWindowShouldClose(engine.window)) {
        engine.run();
        bh.drawCircle(engine.shaderProgram);

        for (auto& ray : rays) {
            ray.draw_ray(engine.shaderProgram);
            ray.step();
        }

        glfwSwapBuffers(engine.window);
        glfwPollEvents();
    }

    engine.cleanup();
    return 0;
}