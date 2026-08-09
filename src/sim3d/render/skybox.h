#pragma once
#include <GL/glew.h>
#include <string>

// Equirectangular background image held as a 2D texture.
// Sampled by direction, so it behaves as an infinitely distant sphere.
class Skybox {
public:
    bool load(const std::string& path);
    void bind(GLuint unit = 0) const;
    void destroy();

    bool valid() const { return tex != 0; }

private:
    GLuint tex = 0;
};
