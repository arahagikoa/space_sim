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
#include <map>


#include "sim3d/app3d.h"
#include "physics/black_hole.h"
#include "sim2d/lensing/ray.h"
#include "sim3d/scene/camera.h"
#include "sim3d/screen_quad.h"
#include "sim3d/render/skybox.h"
#include "sim3d/render/spacetime_grid.h"
#include "sim3d/render/framebuffer.h"
#include "sim3d/render/screenshot.h"


using Clock = std::chrono::high_resolution_clock;

void geodesic(Ray& ray, double rhs[4], const std::vector<BlackHole>& bhs);
void addState(const double a[4], const double b[4], double factor, double out[4]);
void rk4step(Ray& ray, double dlambda, const std::vector<BlackHole>& bhs);
void setupCameraCallbacks(GLFWwindow* window);

float WIDTH = 600.0;
float HEIGHT = 600.0;

constexpr float GRID_EXTENT = 1.6f;

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

struct Params {
    float rs           = 1.0f;
    float diskInner    = 3.0f;    // ISCO for a non-spinning hole
    float diskOuter    = 40.0f;
    int   steps        = 200;
    float renderScale  = 0.75f;   // fraction of window resolution to ray march at
    float exposure     = 2.5f;
    float bloom        = 0.6f;
    bool  doppler      = true;
    bool  showGrid     = true;
    bool  showRing     = false;
};

class KeyLatch {
public:
    bool pressed(GLFWwindow* window, int key) {
        const bool down = glfwGetKey(window, key) == GLFW_PRESS;
        const bool edge = down && !was[key];
        was[key] = down;
        return edge;
    }
private:
    std::map<int, bool> was;
};


static void print_controls() {
    std::cout <<
        "\n  drag / scroll  orbit, zoom\n"
        "  G              spacetime grid\n"
        "  D              Doppler beaming and redshift\n"
        "  R              theoretical shadow edge (M8 check)\n"
        "  - / =          exposure\n"
        "  [ / ]          disk outer radius\n"
        "  , / .          integrator steps\n"
        "  1 / 2          black hole mass\n"
        "  9 / 0          render resolution\n"
        "  P              screenshot\n\n";
}


static void render_bloom(Framebuffer& a, Framebuffer& b, const Framebuffer& scene,
                         GLuint extractProgram, GLuint blurProgram,
                         const ScreenQuad& quad, float exposure) {
    a.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(extractProgram);
    scene.bindTexture(0);
    glUniform1i(glGetUniformLocation(extractProgram, "uScene"), 0);
    glUniform1f(glGetUniformLocation(extractProgram, "uExposure"), exposure);
    glUniform1f(glGetUniformLocation(extractProgram, "uThreshold"), 1.0f);
    quad.draw();

    glUseProgram(blurProgram);
    const GLint srcLoc = glGetUniformLocation(blurProgram, "uSource");
    const GLint dirLoc = glGetUniformLocation(blurProgram, "uDirection");
    const float texelX = 1.0f / a.width();
    const float texelY = 1.0f / a.height();

    for (int i = 0; i < 3; ++i) {
        b.bind();
        a.bindTexture(0);
        glUniform1i(srcLoc, 0);
        glUniform2f(dirLoc, texelX, 0.0f);
        quad.draw();

        a.bind();
        b.bindTexture(0);
        glUniform1i(srcLoc, 0);
        glUniform2f(dirLoc, 0.0f, texelY);
        quad.draw();
    }
}


int main() {
    Engine engine(WIDTH, HEIGHT);
    if (!engine.init()) return -1;

    setupCameraCallbacks(engine.window);

    GLuint quadProgram = engine.CreateShaderProgram(
        "./assets/shaders/sim3d/quad.vert",
        "./assets/shaders/sim3d/quad.frag");
    GLuint gridProgram = engine.CreateShaderProgram(
        "./assets/shaders/sim3d/grid.vert",
        "./assets/shaders/sim3d/grid.frag");
    GLuint extractProgram = engine.CreateShaderProgram(
        "./assets/shaders/sim3d/quad.vert",
        "./assets/shaders/sim3d/bloom_extract.frag");
    GLuint blurProgram = engine.CreateShaderProgram(
        "./assets/shaders/sim3d/quad.vert",
        "./assets/shaders/sim3d/bloom_blur.frag");
    GLuint compositeProgram = engine.CreateShaderProgram(
        "./assets/shaders/sim3d/quad.vert",
        "./assets/shaders/sim3d/composite.frag");

    ScreenQuad quad;
    quad.init();

    Skybox sky;
    if (!sky.load("./assets/images/starmap_2020_4k.png")) return -1;

    Params params;

    Grid grid;
    grid.build(params.rs, params.diskOuter * params.rs * GRID_EXTENT);

    Framebuffer scene, bloomA, bloomB;

    camera.radius = static_cast<double>(params.diskOuter) * params.rs * 1.4;

    glUseProgram(quadProgram);
    glUniform1i(glGetUniformLocation(quadProgram, "uSky"), 0);

    KeyLatch keys;
    print_controls();

    int  frames = 0;
    double fpsClock = glfwGetTime();

    while (!glfwWindowShouldClose(engine.window)) {
        engine.processInput();

        if (keys.pressed(engine.window, GLFW_KEY_G)) params.showGrid = !params.showGrid;
        if (keys.pressed(engine.window, GLFW_KEY_D)) params.doppler  = !params.doppler;
        if (keys.pressed(engine.window, GLFW_KEY_R)) params.showRing = !params.showRing;

        if (glfwGetKey(engine.window, GLFW_KEY_MINUS) == GLFW_PRESS) params.exposure *= 0.98f;
        if (glfwGetKey(engine.window, GLFW_KEY_EQUAL) == GLFW_PRESS) params.exposure *= 1.02f;
        params.exposure = glm::clamp(params.exposure, 0.05f, 50.0f);

        bool rebuildGrid = false;
        if (keys.pressed(engine.window, GLFW_KEY_LEFT_BRACKET)) {
            params.diskOuter = glm::max(params.diskOuter - 4.0f, params.diskInner + 2.0f);
            rebuildGrid = true;
        }
        if (keys.pressed(engine.window, GLFW_KEY_RIGHT_BRACKET)) {
            params.diskOuter = glm::min(params.diskOuter + 4.0f, 200.0f);
            rebuildGrid = true;
        }
        if (keys.pressed(engine.window, GLFW_KEY_1)) {
            params.rs = glm::max(params.rs * 0.8f, 0.1f);
            rebuildGrid = true;
        }
        if (keys.pressed(engine.window, GLFW_KEY_2)) {
            params.rs = glm::min(params.rs * 1.25f, 10.0f);
            rebuildGrid = true;
        }
        if (rebuildGrid) {
            grid.build(params.rs, params.diskOuter * params.rs * GRID_EXTENT);
            std::cout << "r_s " << params.rs
                      << "   disk " << params.diskInner << "-" << params.diskOuter << " r_s\n";
        }

        if (keys.pressed(engine.window, GLFW_KEY_COMMA))
            params.steps = glm::max(params.steps - 25, 25);
        if (keys.pressed(engine.window, GLFW_KEY_PERIOD))
            params.steps = glm::min(params.steps + 25, 2000);

        if (keys.pressed(engine.window, GLFW_KEY_9))
            params.renderScale = glm::max(params.renderScale - 0.125f, 0.25f);
        if (keys.pressed(engine.window, GLFW_KEY_0))
            params.renderScale = glm::min(params.renderScale + 0.125f, 1.0f);

        const int windowW = (int)WIDTH;
        const int windowH = (int)HEIGHT;
        const int renderW = glm::max((int)(WIDTH  * params.renderScale), 1);
        const int renderH = glm::max((int)(HEIGHT * params.renderScale), 1);

        scene.create(renderW, renderH, true);
        bloomA.create(glm::max(renderW / 2, 1), glm::max(renderH / 2, 1), false);
        bloomB.create(glm::max(renderW / 2, 1), glm::max(renderH / 2, 1), false);

        const glm::vec3 camPos = camera.get_camera_position();
        const glm::vec3 frwd   = camera.get_forward();
        const glm::vec3 right  = camera.get_right();
        const glm::vec3 up     = camera.get_up();
        const glm::mat3 basis(right, up, frwd);

         const glm::mat4 viewProj =
            glm::perspective(glm::radians(90.0f), WIDTH / HEIGHT, 0.05f, 4000.0f) *
            glm::lookAt(camPos, glm::vec3(0.0f), camera.world_up);

        scene.bind();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_ALWAYS);

        glUseProgram(quadProgram);
        sky.bind(0);
        glUniformMatrix3fv(glGetUniformLocation(quadProgram, "basis"), 1, GL_FALSE, glm::value_ptr(basis));
        glUniformMatrix4fv(glGetUniformLocation(quadProgram, "uViewProj"), 1, GL_FALSE, glm::value_ptr(viewProj));
        glUniform3fv(glGetUniformLocation(quadProgram, "cameraPos"), 1, glm::value_ptr(camPos));
        glUniform1f(glGetUniformLocation(quadProgram, "aspect_ratio"), WIDTH / HEIGHT);
        glUniform1f(glGetUniformLocation(quadProgram, "uFovY"), 90.0f);
        glUniform1f(glGetUniformLocation(quadProgram, "uRs"), params.rs);
        glUniform1f(glGetUniformLocation(quadProgram, "uDiskInner"), params.diskInner * params.rs);
        glUniform1f(glGetUniformLocation(quadProgram, "uDiskOuter"), params.diskOuter * params.rs);
        glUniform1i(glGetUniformLocation(quadProgram, "uSteps"), params.steps);
        glUniform1f(glGetUniformLocation(quadProgram, "uTime"), (float)glfwGetTime());
        glUniform1i(glGetUniformLocation(quadProgram, "uDoppler"), params.doppler);
        glUniform1i(glGetUniformLocation(quadProgram, "uShowShadowRing"), params.showRing);
        quad.draw();

        if (params.showGrid) {
            glDepthFunc(GL_LESS);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            const float gridOuter = params.diskOuter * params.rs * GRID_EXTENT;

            glUseProgram(gridProgram);
            glUniformMatrix4fv(glGetUniformLocation(gridProgram, "uViewProj"), 1, GL_FALSE, glm::value_ptr(viewProj));
            glUniform3f(glGetUniformLocation(gridProgram, "uColor"), 0.10f, 0.32f, 0.55f);
            glUniform1f(glGetUniformLocation(gridProgram, "uFadeStart"), gridOuter * 0.55f);
            glUniform1f(glGetUniformLocation(gridProgram, "uFadeEnd"), gridOuter);
            grid.draw();

            glDisable(GL_BLEND);
        }

        glDisable(GL_DEPTH_TEST);
        render_bloom(bloomA, bloomB, scene, extractProgram, blurProgram, quad, params.exposure);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, windowW, windowH);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(compositeProgram);
        scene.bindTexture(0);
        bloomA.bindTexture(1);
        glUniform1i(glGetUniformLocation(compositeProgram, "uScene"), 0);
        glUniform1i(glGetUniformLocation(compositeProgram, "uBloom"), 1);
        glUniform1f(glGetUniformLocation(compositeProgram, "uExposure"), params.exposure);
        glUniform1f(glGetUniformLocation(compositeProgram, "uBloomStrength"), params.bloom);
        quad.draw();

        if (keys.pressed(engine.window, GLFW_KEY_P)) save_screenshot(windowW, windowH);

        glfwSwapBuffers(engine.window);
        glfwPollEvents();

        if (++frames >= 60) {
            const double now = glfwGetTime();
            std::cout << std::fixed << std::setprecision(1)
                      << frames / (now - fpsClock) << " fps   "
                      << renderW << "x" << renderH << "   "
                      << params.steps << " steps   exposure " << params.exposure << "\r"
                      << std::flush;
            frames = 0;
            fpsClock = now;
        }
    }

    grid.destroy();
    scene.destroy();
    bloomA.destroy();
    bloomB.destroy();
    sky.destroy();
    quad.destroy();
    engine.cleanup();
    return 0;
}