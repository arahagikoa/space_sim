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

#include "engine.h"
#include "black_hole.h"
#include "ray.h"
#include "camera.h"
#include "grid.h"


using Clock = std::chrono::high_resolution_clock;

void geodesic(Ray& ray, double rhs[4], BlackHole bh);
void addState(const double a[4], const double b[4], double factor, double out[4]);
void rk4step(Ray& ray, double dlambda, BlackHole bh);
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


void geodesic(Ray& ray, double rhs[4], BlackHole bh) {
    double r = ray.r;
    double theta = ray.theta;
    double dr = ray.dr;
    double dtheta = ray.dtheta;
    double dphi = ray.dphi;
    double E = ray.E;
    double r_s = bh.r_s;

    double f = 1.0 - r_s / r;
    double dt_dlambda = E / f;

    // First derivatives
    rhs[0] = dr;
    rhs[1] = dtheta;
    rhs[2] = dphi;

    // Second derivatives (from 3D Schwarzschild null geodesics):
    rhs[3] =
        -(r_s / (2 * r * r)) * f * dt_dlambda * dt_dlambda
        + (r_s / (2 * r * r * f)) * dr * dr
        + r * (dtheta * dtheta + sin(theta) * sin(theta) * dphi * dphi);

    rhs[4] =
        -(2.0 / r) * dr * dtheta
        + sin(theta) * cos(theta) * dphi * dphi;

    rhs[5] =
        -(2.0 / r) * dr * dphi
        - 2.0 * cos(theta) / sin(theta) * dtheta * dphi;
}

void addState(const double a[4], const double b[4], double factor, double out[4]) {
    for (int i = 0; i < 4; i++)
        out[i] = a[i] + b[i] * factor;
}

void rk4step(Ray& ray, double dlambda, BlackHole bh) {
    double y0[6] = { ray.r, ray.phi, ray.theta, ray.dr, ray.dphi, ray.dtheta};
    double k1[6], k2[6], k3[6], k4[6], temp[6];

    geodesic(ray, k1, bh);

    addState(y0, k1, dlambda / 2.0, temp);
    Ray r2 = ray;
    r2.r = temp[0]; r2.phi = temp[1]; r2.theta = temp[2], r2.dr = temp[3]; r2.dphi = temp[4], r2.dtheta = temp[5];
    geodesic(r2, k2, bh);

    addState(y0, k2, dlambda / 2.0, temp);
    Ray r3 = ray;
    r3.r = temp[0]; r3.phi = temp[1]; r3.theta = temp[2], r3.dr = temp[3]; r3.dphi = temp[4], r3.dtheta = temp[5];
    geodesic(r3, k3, bh);

    addState(y0, k3, dlambda, temp);
    Ray r4 = ray;
    r4.r = temp[0]; r4.phi = temp[1]; r4.theta = temp[2], r4.dr = temp[3]; r4.dphi = temp[4], r4.dtheta = temp[5];
    geodesic(r4, k4, bh);

    for (int i = 0; i < 4; i++) {
        y0[i] += (dlambda / 6.0) * (k1[i] + 2.0 * k2[i] + 2.0 * k3[i] + k4[i]);
    }

    ray.r = y0[0];
    ray.phi = y0[1];
    ray.theta = y0[2];
    ray.dr = y0[3];
    ray.dphi = y0[4];
    ray.dtheta = y0[5];

}

int main() {
    Engine engine(WIDTH, HEIGHT);



    if (!engine.init()) return -1;
    engine.shaderProgram = engine.CreateShaderProgram();
    GLuint gridShaderProgram = engine.CreateShaderProgram("./shaders/grid.vert", "./shaders/grid.frag");
    setupCameraCallbacks(engine.window);


    Grid grid(engine.WIDTH);
    BlackHole bh(glm::vec3(0.0f, 0.0f, 0.0f), 5.0e27);


    double dlambda = 1;
    double lastTime = glfwGetTime();
    int nbFrames = 0;


    while (!glfwWindowShouldClose(engine.window)) {

        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
            // printf and reset timer
            printf("%f ms/frame\n", 1000.0 / double(nbFrames)); // 16.667 ms / fram -> 60 FPS
            nbFrames = 0;
            lastTime += 1.0;
        }

        engine.run();

        glm::mat4 view = camera.get_view_matrix(); // recalculating position, creating lookAt matrix
        glm::mat4 projection = camera.projection;


        glUseProgram(engine.shaderProgram);

        GLuint viewLoc = glGetUniformLocation(engine.shaderProgram, "view");
        GLuint projLoc = glGetUniformLocation(engine.shaderProgram, "projection");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);

        bh.drawCircle(engine.shaderProgram);
        glDisable(GL_DEPTH_TEST);

        camera.update();
        glfwSwapBuffers(engine.window);
        glfwPollEvents();
    }

    engine.cleanup();
    return 0;
}