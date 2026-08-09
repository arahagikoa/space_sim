#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "sim3d/render/skybox.h"

#include <iostream>


bool Skybox::load(const std::string& path) {
    // Image files are stored top-down, GL texture space runs bottom-up.
    stbi_set_flip_vertically_on_load(true);

    int w = 0, h = 0, channels = 0;
    unsigned char* pixels = stbi_load(path.c_str(), &w, &h, &channels, 3);
    if (!pixels) {
        std::cerr << "Skybox: failed to load " << path
                  << " (" << stbi_failure_reason() << ")\n";
        return false;
    }
    std::cout << "Skybox: loaded " << path << " " << w << "x" << h
              << " (" << channels << " channels in file)\n";

    if (w != 2 * h) {
        std::cerr << "Skybox: warning - " << w << "x" << h
                  << " is not 2:1, equirectangular mapping expects it\n";
    }

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    // Longitude wraps around the full 360 degrees; latitude does not - clamping
    // stops the top and bottom of the sky mirroring back on themselves.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Rows are 3 bytes per texel and the width may not be a multiple of 4.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // GL_RGB8 passes the file's values straight through. Switching this to
    // GL_SRGB8 would linearise on sample, which also requires GL_FRAMEBUFFER_SRGB
    // on the way out - worth doing once there is real light transport to get right.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(pixels);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "Skybox: GL error 0x" << std::hex << err << std::dec
                  << " during upload\n";
        return false;
    }
    return true;
}


void Skybox::bind(GLuint unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, tex);
}


void Skybox::destroy() {
    if (tex) {
        glDeleteTextures(1, &tex);
        tex = 0;
    }
}
