#pragma once
#include <GL/glew.h>
#include <string>
class Skybox {
public:
    bool load(const std::string& path);
    void bind(GLuint unit = 0) const;
    void destroy();

    bool valid() const { return tex != 0; }

private:
    GLuint tex = 0;
};
