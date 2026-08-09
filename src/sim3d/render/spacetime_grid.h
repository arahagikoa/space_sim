#pragma once

#include <GL/glew.h>


class Grid {
public:

    void build(float schwarzschildRadius, float outerRadius);
    void draw() const;
    void destroy();

private:
    GLuint  vao = 0, vbo = 0;
    GLsizei vertexCount = 0;
};
