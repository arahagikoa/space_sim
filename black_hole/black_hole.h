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
#ifndef C_VEL
#define C_VEL 299792458.0
#endif

class BlackHole {
public:
    float M;
    float r_s;

    BlackHole(float x, float y, float m);
    void drawCircle(GLuint shaderProgram);

private:
    glm::vec2 pos;

    GLuint VAO, VBO;
    int vertexCount = 0;

    void setupCircle();
};

#endif // !BLACK_HOLE_H