#include "black_hole.h"

BlackHole::BlackHole(float x, float y, float m) {
    pos.x = x;
    pos.y = y;
    M = m;
    double c = 299792458.0;
    double G = 6.67430e-11;

    r_s = 2.0 * G * m / (c * c);

    setupCircle();
}

void BlackHole::setupCircle() {
    std::vector<float> vertices;

    vertices.push_back(pos.x);
    vertices.push_back(pos.y);

    int segments = 100; 

    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float x = pos.x + cos(angle) * r_s; 
        float y = pos.y + sin(angle) * r_s; 
        vertices.push_back(x);
        vertices.push_back(y);
    }

    vertexCount = segments + 2;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void BlackHole::drawCircle(GLuint shaderProgram) {
    glUseProgram(shaderProgram);
    GLint colorLoc = glGetUniformLocation(shaderProgram, "uColor");
    if (colorLoc != -1)
        glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
    glBindVertexArray(0);
}