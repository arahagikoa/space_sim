#include "sim3d/render/spacetime_grid.h"

#include <cmath>
#include <vector>

namespace {

constexpr int RINGS    = 28;   // rings from the throat outward
constexpr int SPOKES   = 48;   // radial lines
constexpr int SEGMENTS = 128;  // segments per ring

constexpr float TWO_PI = 6.28318530718f;

float embedding_height(float r, float rs, float outer) {
    return 2.0f * (std::sqrt(rs * (r - rs)) - std::sqrt(rs * (outer - rs)));
}

float ring_radius(int i, float rs, float outer) {
    const float u = static_cast<float>(i) / static_cast<float>(RINGS);
    return rs + (outer - rs) * u * u;
}

} 


void Grid::build(float schwarzschildRadius, float outerRadius) {
    const float rs = schwarzschildRadius;

    std::vector<float> verts;
    verts.reserve(((RINGS + 1) * SEGMENTS + SPOKES * RINGS) * 6);

    auto emit = [&](float r, float angle) {
        verts.push_back(r * std::cos(angle));
        verts.push_back(embedding_height(r, rs, outerRadius));
        verts.push_back(r * std::sin(angle));
    };

    for (int i = 0; i <= RINGS; ++i) {
        const float r = ring_radius(i, rs, outerRadius);

        for (int j = 0; j < SEGMENTS; ++j) {
            emit(r, TWO_PI * j / SEGMENTS);
            emit(r, TWO_PI * (j + 1) / SEGMENTS);
        }
    }

    for (int k = 0; k < SPOKES; ++k) {
        const float angle = TWO_PI * k / SPOKES;

        for (int i = 0; i < RINGS; ++i) {
            emit(ring_radius(i, rs, outerRadius), angle);
            emit(ring_radius(i + 1, rs, outerRadius), angle);
        }
    }

    vertexCount = static_cast<GLsizei>(verts.size() / 3);

    if (!vao) {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void Grid::draw() const {
    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, vertexCount);
    glBindVertexArray(0);
}


void Grid::destroy() {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    vao = vbo = 0;
    vertexCount = 0;
}
