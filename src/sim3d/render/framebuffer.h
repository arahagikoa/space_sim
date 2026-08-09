#pragma once

#include <GL/glew.h>

class Framebuffer {
public:
    bool create(int width, int height, bool withDepth);

    void bind() const;
    void bindTexture(GLuint unit) const;
    void destroy();

    int width()  const { return w; }
    int height() const { return h; }

private:
    GLuint fbo = 0, colour = 0, depth = 0;
    int    w = 0, h = 0;
    bool   hasDepth = false;
};
