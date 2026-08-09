#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "sim3d/render/screenshot.h"

#include <GL/glew.h>

#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>


std::string save_screenshot(int width, int height) {
    if (width < 1 || height < 1) return {};

    std::vector<unsigned char> pixels(static_cast<size_t>(width) * height * 3);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    const std::time_t now = std::time(nullptr);
    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &now);
#else
    localtime_r(&now, &local);
#endif

    std::ostringstream name;
    name << "blackhole_" << std::put_time(&local, "%Y%m%d_%H%M%S") << ".png";

    stbi_flip_vertically_on_write(1);

    if (!stbi_write_png(name.str().c_str(), width, height, 3, pixels.data(), width * 3)) {
        std::cerr << "Screenshot: failed to write " << name.str() << "\n";
        return {};
    }

    std::cout << "Screenshot: wrote " << name.str() << "\n";
    return name.str();
}
