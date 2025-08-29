#include "black_hole.h"

BlackHole::BlackHole(glm::vec3 position, double m) {
    
    this->pos = position;
    this->M = m;
    this->r_s = 2.0 * G * m / (c * c);

    setupCircle();
}

void BlackHole::setupCircle() {
    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;

    const float PI = 3.14159265359f;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uv;
    std::vector<unsigned int> indices;

    // Generate sphere vertices
    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;

            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            glm::vec3 posScaled = pos + glm::vec3(xPos, yPos, zPos) * (float)r_s;

            positions.push_back(posScaled);
            normals.push_back(glm::normalize(glm::vec3(xPos, yPos, zPos)));
            uv.push_back(glm::vec2(xSegment, ySegment));
        }
    }

    // Generate index order (triangle strip style)
    bool oddRow = false;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
        if (!oddRow) { // even rows
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
                indices.push_back(y * (X_SEGMENTS + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        }
        else { // odd rows
            for (int x = X_SEGMENTS; x >= 0; --x) {
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }

    indexCount = static_cast<unsigned int>(indices.size());

    // Pack into a single float array
    std::vector<float> data;
    for (unsigned int i = 0; i < positions.size(); ++i) {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);

        data.push_back(normals[i].x);
        data.push_back(normals[i].y);
        data.push_back(normals[i].z);

        data.push_back(uv[i].x);
        data.push_back(uv[i].y);
    }

    // OpenGL buffer setup
    unsigned int vbo, ebo;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    unsigned int stride = (3 + 3 + 2) * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

    glBindVertexArray(0);
}

void BlackHole::drawCircle(GLuint shaderProgram) {
    glUseProgram(shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    GLint colorLoc = glGetUniformLocation(shaderProgram, "uColor");
    if (colorLoc != -1)
        glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f); // Red sphere

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


bool BlackHole::intercept(glm::vec3 pos) {
    float dx = pos.x - pos.x;
    float dy = pos.y - pos.y;
    float dz = pos.z - pos.z;
    float dist2 = dx * dx + dy * dy + dz * dz;

    return dist2 < r_s * r_s;
}