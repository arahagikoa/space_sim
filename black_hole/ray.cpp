#include "ray.h"


Ray::Ray(glm::vec2 pos, glm::vec2 dir) {
	x = pos.x;
	y = pos.y;
	this->dir = dir;
	// there probability initialize empty vector of screen/step size

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void Ray::draw_ray(GLuint shaderProgram) {
    glUseProgram(shaderProgram);

    GLint colorLoc = glGetUniformLocation(shaderProgram, "uColor");

    glPointSize(5.0f);

    float pointPos[2] = { x, y };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointPos), pointPos, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);
    glDrawArrays(GL_POINTS, 0, 1);

    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    size_t N = trail.size();
    if (N < 2) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(2.0f);

    GLuint VAO_trail, VBO_trail;
    glGenVertexArrays(1, &VAO_trail);
    glGenBuffers(1, &VBO_trail);

    glBindVertexArray(VAO_trail);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_trail);
    glBufferData(GL_ARRAY_BUFFER, N * sizeof(glm::vec2), trail.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f); // white color, but no alpha here yet
    glDrawArrays(GL_LINE_STRIP, 0, N);

    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glDeleteBuffers(1, &VBO_trail);
    glDeleteVertexArrays(1, &VAO_trail);

    glDisable(GL_BLEND);
}


void Ray::step() {
	x += dir.x;
	y += dir.y;

    trail.push_back(glm::vec2(x, y));
}