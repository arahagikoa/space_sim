#pragma once
#ifndef BLACK_HOLE_H
#define BLACK_HOLE_H


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>
#include <iostream>
#include <vector>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef PLANCK_LEN
#define PLANCK_LEN 1.616255e-35
#endif

class BlackHole {
public:
    BlackHole(float x, float y, float m);
    void drawCircle(GLuint shaderProgram);
private:
    glm::vec2 pos;
    float M;
    float r_s;

    GLuint VAO = 0;
    GLuint VBO = 0;
    int vertexCount = 0;

    void setupCircle();
};

#endif // !BLACK_HOLE_H