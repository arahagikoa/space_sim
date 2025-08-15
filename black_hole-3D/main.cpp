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
#include "camera.h"


using Clock = std::chrono::high_resolution_clock;

void geodesic(Ray& ray, double rhs[4], const std::vector<BlackHole>& bhs);
void addState(const double a[4], const double b[4], double factor, double out[4]);
void rk4step(Ray& ray, double dlambda, const std::vector<BlackHole>& bhs);
void setupCameraCallbacks(GLFWwindow* window);

int WIDTH = 600;
int HEIGHT = 600;

std::vector<Ray> rays;

Camera camera;


void setupCameraCallbacks(GLFWwindow* window) {
    glfwSetWindowUserPointer(window, &camera);

    glfwSetMouseButtonCallback(window, [](GLFWwindow* win, int button, int action, int mods) {
        Camera* cam = (Camera*)glfwGetWindowUserPointer(win);
        cam->process_mouse_buttons(button, action, mods, win);
        });

    glfwSetCursorPosCallback(window, [](GLFWwindow* win, double x, double y) {
        Camera* cam = (Camera*)glfwGetWindowUserPointer(win);
        cam->process_mouse_move(x, y);
        });
}


void geodesic(Ray& ray, double rhs[4], const std::vector<BlackHole>& bhs) {
    double r = ray.r;
    double phi = ray.phi;
    double dr = ray.dr;
    double dphi = ray.dphi;
    double E = ray.E;

    double f_total = 1.0;
    for (auto& bh : bhs) {
        f_total *= (1.0 - bh.r_s / r); // naive multiplicative effect
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

int main() {
    Engine engine(WIDTH, HEIGHT);



    if (!engine.init()) return -1;
    engine.shaderProgram = engine.CreateShaderProgram();
    setupCameraCallbacks(engine.window);



    BlackHole bh(glm::vec3(0.0f, 0.0f, 0.0f), 5.0e27);


    double dlambda = 1;


    while (!glfwWindowShouldClose(engine.window)) {
        engine.run();
        glm::vec3 camPos = camera.get_camera_position();

        glm::mat4 view = glm::lookAt(
            camPos,                      // Camera position
            glm::vec3(0.0f, 0.0f, 0.0f), // Look at the black hole
            glm::vec3(0.0f, 1.0f, 0.0f)  // Up direction
        );

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)WIDTH / (float)HEIGHT,
            0.1f,
            1e25f
        );

        glUseProgram(engine.shaderProgram);

        GLuint viewLoc = glGetUniformLocation(engine.shaderProgram, "view");
        GLuint projLoc = glGetUniformLocation(engine.shaderProgram, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


        bh.drawCircle(engine.shaderProgram);
        // bh2.drawCircle(engine.shaderProgram);
        std::vector<BlackHole> blackHoles = { bh };
        //for (auto& ray : rays) {

        //    if (ray.r > bh.r_s) {
        //        rk4step(ray, dlambda, blackHoles);
        //        ray.step(bh.r_s, dlambda);
        //    }
        //    ray.draw_ray(engine.shaderProgram);
        //}

        glfwSwapBuffers(engine.window);
        glfwPollEvents();
    }

    engine.cleanup();
    return 0;
}