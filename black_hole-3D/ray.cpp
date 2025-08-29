#include "ray.h"

Ray::Ray(glm::vec3 pos, glm::vec3 dir, double r_s) {
    x = pos.x;
    y = pos.y;
    z = pos.z;

    this->dir = dir;

    this->r = std::sqrt(x * x + y * y + z * z);
    this->phi = std::atan2(y, x);
    this->theta = std::acos(x / r);

    double dx = dir.x, dy = dir.y, dz = dir.z; // for convenienve

    dr = sin(theta) * cos(phi) * dx + sin(theta) * sin(phi) * dy + cos(theta) * dz;
    dtheta = cos(theta) * cos(phi) * dx + cos(theta) * sin(phi) * dy - sin(theta) * dz;
    dtheta /= r;
    dphi = -sin(phi) * dx + cos(phi) * dy;
    dphi /= (r * sin(theta));

    L = r * r * sin(theta) * dphi;
    double f = 1.0 - r_s / r;
    double dt_dlambda = sqrt((dr * dr) / f + r * r * dtheta * dtheta + r * r * sin(theta) * sin(theta) * dphi * dphi);
    E = f * dt_dlambda;


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
    if (this->r <= r_s) return;

    this->x = r * sin(theta) * cos(phi);
    this->y = r * sin(theta) * sin(phi);
    this->z = r * cos(theta);
}