#include "ray.h"

Ray::Ray(glm::vec2 pos, glm::vec2 dir, double r_s) {
    x = pos.x;
    y = pos.y;
    this->dir = dir;

    float centerX = 400.0f;
    float centerY = 400.0f; 
    //std::cout << "X" << x << std::endl;
    //std::cout << "Y" << y << std::endl;

    double metersPerPixel = 1e-1; // same scale as BlackHole
    this->r = std::sqrt(x * x + y * y);
    this->phi = std::atan2(y, x);

    float cosP = cos(phi);
    float sinP = sin(phi);

    dr = dir.x * cosP + dir.y * sinP; 
    dphi = (-dir.x * sinP + dir.y * cosP) / r; 
    L = r * r * dphi;
    double f = 1.0 - r_s / r;
    double dt_dlambda = sqrt((dr * dr) / (f * f) + (r * r * dphi * dphi) / f);
    E = f * dt_dlambda;
    //std::cout << "R ray  " << this->r << std::endl;
    glGenVertexArrays(1, &VAO_point);
    glGenBuffers(1, &VBO_point);
    glBindVertexArray(VAO_point);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_point);
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenVertexArrays(1, &VAO_trail);
    glGenBuffers(1, &VBO_trail);
    glBindVertexArray(VAO_trail);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_trail);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Ray::draw_ray(GLuint shaderProgram) {
    glUseProgram(shaderProgram);
    GLint colorLoc = glGetUniformLocation(shaderProgram, "uColor");

    if (colorLoc != -1)
        glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);


    float pointPos[2] = { x, y };
    glBindBuffer(GL_ARRAY_BUFFER, VBO_point);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointPos), pointPos);
    glBindVertexArray(VAO_point);
    glPointSize(3.0f);
    glDrawArrays(GL_POINTS, 0, 1);
    glBindVertexArray(0);
    size_t N = trail.size();
    if (trail.size() < 2) return;


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_trail);
    glBufferData(GL_ARRAY_BUFFER, trail.size() * sizeof(glm::vec2), trail.data(), GL_DYNAMIC_DRAW);
    glBindVertexArray(VAO_trail);
    glDrawArrays(GL_LINE_STRIP, 0, trail.size());
    glBindVertexArray(0);
    glDisable(GL_BLEND);
}

void Ray::step(double r_s, double dlambda) {
    //std::cout << "R_L" << this->r << std::endl;
    if (this->r <= r_s) {
        return;
    }

    float centerX = 400.0f;
    float centerY = 300.0f;

    x = 0.0 + r * cos(phi);
    y = 0.0 + r * sin(phi);
    // std::cout << "X" << x << std::endl;


    trail.push_back(glm::vec2(x, y));
    if (trail.size() > 500) {
        trail.erase(trail.begin());
    }
}