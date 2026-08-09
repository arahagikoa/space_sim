#pragma once
#include <GL/glew.h>

class ScreenQuad {
public:
    void init();
    void draw() const;
    void destroy();
private:
    GLuint VAO = 0, VBO = 0;
};