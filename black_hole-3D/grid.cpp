#include "grid.h"


Grid::Grid(float width) {
    setupGrid(width, width);

}


void Grid::setupGrid(float WIDTH, float HEIGHT) {
    std::vector<float> vertices;

    int gridLines = 20;
    float stepX = 2.0 / gridLines;
    float stepY = 2.0 / gridLines;


    // vertical lines
    for (int i = 0; i <= gridLines; i++) {
        float x = -2.0 / 2.0f + i * stepX;


        vertices.push_back(x); vertices.push_back(-HEIGHT / 2.0f);
        vertices.push_back(x); vertices.push_back(HEIGHT / 2.0f);
    }

    // horizontal lines
    for (int j = 0; j <= gridLines; j++) {
        float y = -2.0 / 2.0f + j * stepY;
        vertices.push_back(-WIDTH / 2.0f); vertices.push_back(y);
        vertices.push_back(WIDTH / 2.0f); vertices.push_back(y);
    }

    vertexCount = vertices.size() / 2;

    glGenVertexArrays(1, &GridVAO);
    glGenBuffers(1, &GridVBO);
    glBindVertexArray(GridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, GridVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Grid::drawGrid(GLuint shaderProgram) {
    glUseProgram(shaderProgram);


    GLint zLoc = glGetUniformLocation(shaderProgram, "uZ");

    glUniform1f(zLoc, -0.1f);

    GLint colorLoc = glGetUniformLocation(shaderProgram, "uColor");

    if (colorLoc == -1) {
        std::cerr << "uColor not found in shader\n";
    }
    else {
        glUniform3f(colorLoc, 0.5f, 0.5f, 0.5f); // idk what this color is xd
    }
    glBindVertexArray(GridVAO);
    glDrawArrays(GL_LINES, 0, vertexCount);
    glBindVertexArray(0);
}