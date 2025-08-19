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
double r_s = 0.0f;


void geodesic(Ray& ray, double rhs[4], const std::vector<BlackHole>& bhs);
void addState(const double a[4], const double b[4], double factor, double out[4]);
void rk4step(Ray& ray, double dlambda, const std::vector<BlackHole>& bhs);

int WIDTH = 800;
int HEIGHT = 800;
std::vector<Ray> rays;


void geodesic(Ray& ray, double rhs[4], const std::vector<BlackHole>& bhs) {
    double r = ray.r;
    double phi = ray.phi;
    double dr = ray.dr;
    double dphi = ray.dphi;
    double E = ray.E;

    double f_total = 1.0;
    for (auto& bh : bhs) {
        f_total *= (1.0 - bh.r_s / r);
    }

    rhs[0] = dr;
    rhs[1] = dphi;
    double dt_dlambda = E / f_total;
    rhs[2] = 0;
    for (auto& bh : bhs) {
        rhs[2] += -(bh.r_s / (2 * r * r)) * f_total * (dt_dlambda * dt_dlambda)
            + (bh.r_s / (2 * r * r * f_total)) * (dr * dr)
            + (r - bh.r_s) * (dphi * dphi);
    }
    rhs[3] = -2.0 * dr * dphi / r;
}

void addState(const double a[4], const double b[4], double factor, double out[4]) {
    for (int i = 0; i < 4; i++)
        out[i] = a[i] + b[i] * factor;
}

void rk4step(Ray& ray, double dlambda, const std::vector<BlackHole>& bhs) {
    double y0[4] = { ray.r, ray.phi, ray.dr, ray.dphi };
    double k1[4], k2[4], k3[4], k4[4], temp[4];

    geodesic(ray, k1, bhs);

    addState(y0, k1, dlambda / 2.0, temp);
    Ray r2 = ray;
    r2.r = temp[0]; r2.phi = temp[1]; r2.dr = temp[2]; r2.dphi = temp[3];
    geodesic(r2, k2, bhs);

    addState(y0, k2, dlambda / 2.0, temp);
    Ray r3 = ray;
    r3.r = temp[0]; r3.phi = temp[1]; r3.dr = temp[2]; r3.dphi = temp[3];
    geodesic(r3, k3, bhs);

    addState(y0, k3, dlambda, temp);
    Ray r4 = ray;
    r4.r = temp[0]; r4.phi = temp[1]; r4.dr = temp[2]; r4.dphi = temp[3];
    geodesic(r4, k4, bhs);

    for (int i = 0; i < 4; i++) {
        y0[i] += (dlambda / 6.0) * (k1[i] + 2.0 * k2[i] + 2.0 * k3[i] + k4[i]);
    }

    ray.r = y0[0];
    ray.phi = y0[1];
    ray.dr = y0[2];
    ray.dphi = y0[3];
}

bool leftMousePressed = false;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    double xpos, ypos;
    double ray_xpos, ray_ypos;

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            leftMousePressed = true;
        }
   
        else if (action == GLFW_RELEASE) {
            glfwGetCursorPos(window, &xpos, &ypos);


            ray_xpos = (xpos - (WIDTH / 2.0f)) / (WIDTH / 2.0f);
            ray_ypos = -(ypos - (HEIGHT / 2.0f)) / (HEIGHT / 2.0f);

            leftMousePressed = false;
            glm::vec2 startPos = glm::vec2(
                ray_xpos,
                ray_ypos
            );

            glm::vec2 velocity = glm::vec2(
                0.1f,
                0.0f
            );

            rays.push_back(Ray(startPos, velocity, r_s));
            if (rays.size() > 20) {
                rays.erase(rays.begin());

            }
        }
           

    }
}


int main() {
    Engine engine(WIDTH, HEIGHT);

    if (!engine.init()) return -1;
    engine.shaderProgram = engine.CreateShaderProgram();

    //glfwSetCursorPosCallback(engine.window, cursor_position_callback);
    glfwSetMouseButtonCallback(engine.window, mouse_button_callback);


    BlackHole bh(0.0, 0.0, 1.0e26);
    r_s = bh.r_s;

    double dlambda = 0.1;
    for (int i = 0; i < 10; i += 1) {
        glm::vec2 startPos = glm::vec2(
            -1,
            i / 10.0f
        );

        glm::vec2 velocity = glm::vec2(
            0.1f,
            0.0f
        );

        //rays.push_back(Ray(startPos, velocity, bh.r_s));
    }

    while (!glfwWindowShouldClose(engine.window)) {
        engine.run();
        //std::cout << "Left mou pressed   " << leftMousePressed << std::endl;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        bh.drawCircle(engine.shaderProgram);
        glEnable(GL_DEPTH_TEST);

        std::vector<BlackHole> blackHoles = { bh };
        for (auto& ray : rays) {
           
            if (ray.r > bh.r_s) {
                rk4step(ray, dlambda, blackHoles);
                ray.step(bh.r_s, dlambda);
            }
            ray.draw_ray(engine.shaderProgram);
        }

        glfwSwapBuffers(engine.window);
        glfwPollEvents();
    }

    engine.cleanup();
    return 0;
}