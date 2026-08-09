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


#include "sim3d/app3d.h"
#include "physics/black_hole.h"
#include "sim2d/lensing/ray.h"
#include "sim3d/scene/camera.h"
#include "sim3d/screen_quad.h"
#include "sim3d/render/skybox.h"


using Clock = std::chrono::high_resolution_clock;

void geodesic(Ray& ray, double rhs[4], const std::vector<BlackHole>& bhs);
void addState(const double a[4], const double b[4], double factor, double out[4]);
void rk4step(Ray& ray, double dlambda, const std::vector<BlackHole>& bhs);
void setupCameraCallbacks(GLFWwindow* window);

float WIDTH = 600.0;
float HEIGHT = 600.0;

// Geometrised units: the Schwarzschild radius is 1. For a non-spinning hole the
// innermost stable circular orbit sits at 3 r_s, which is where a real disk ends.
constexpr float DISK_INNER = 3.0f;
constexpr float DISK_OUTER = 12.0f;

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

    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        Camera* camera = (Camera*)glfwGetWindowUserPointer(window);
        camera->process_scroll(window, xoffset, yoffset);
        });

    std::cout << "Properly setup camera!" << std::endl;
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
    
    setupCameraCallbacks(engine.window);

    GLuint quadProgram = engine.CreateShaderProgram(
        "./assets/shaders/sim3d/quad.vert",
        "./assets/shaders/sim3d/quad.frag");

    ScreenQuad quad;
    quad.init();

    Skybox sky;
    if (!sky.load("./assets/images/starmap_2020_4k.png")) return -1;

    glDisable(GL_DEPTH_TEST);

    // Frame the disk: far enough out to see the whole annulus at 90 degrees.
    camera.radius = 30.0;

    glUseProgram(quadProgram);
    const GLint AspectLoc  = glGetUniformLocation(quadProgram, "aspect_ratio");
    const GLint fovLoc     = glGetUniformLocation(quadProgram, "uFovY");
    const GLint basisLoc   = glGetUniformLocation(quadProgram, "basis");
    const GLint skyLoc     = glGetUniformLocation(quadProgram, "uSky");
    const GLint camPosLoc  = glGetUniformLocation(quadProgram, "cameraPos");
    const GLint innerLoc   = glGetUniformLocation(quadProgram, "uDiskInner");
    const GLint outerLoc   = glGetUniformLocation(quadProgram, "uDiskOuter");

    glUniform1i(skyLoc, 0);
    glUniform1f(innerLoc, DISK_INNER);
    glUniform1f(outerLoc, DISK_OUTER);

    while (!glfwWindowShouldClose(engine.window)) {
        engine.run();

        glm::vec3 camPos = camera.get_camera_position();
        glm::vec3 frwd = camera.get_forward();
        glm::vec3 right = camera.get_right();
        glm::vec3 up = camera.get_up();
        glm::mat3 basis(right, up, frwd);

        glUseProgram(quadProgram);
        sky.bind(0);

        glUniformMatrix3fv(basisLoc, 1, GL_FALSE, glm::value_ptr(basis));
        glUniform3fv(camPosLoc, 1, glm::value_ptr(camPos));
        glUniform1f(AspectLoc, WIDTH / HEIGHT);
        glUniform1f(fovLoc, 90);

        quad.draw();

        glfwSwapBuffers(engine.window);
        glfwPollEvents();
    }

    sky.destroy();
    quad.destroy();
    engine.cleanup();
    return 0;
}