#pragma once

#ifndef BLACK_HOLE_H
#define BLACK_HOLE_H


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <cmath>
#include <iostream>
#include <vector>


class BlackHole {
public:
    double M;
    double r_s;
    glm::vec3 pos;

    double c = 299792458.0;
    double G = 6.67430e-11;

    unsigned int indexCount;

    BlackHole(glm::vec3 poition, double m);
    void drawCircle(GLuint shaderProgram);

private:

    GLuint VAO, VBO;
    int vertexCount = 0;

    void setupCircle();
    bool intercept(glm::vec3 pos);
};

#endif // !BLACK_HOLE_H