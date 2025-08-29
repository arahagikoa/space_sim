#include "engine.h"

bool Engine::init() {
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



    window = glfwCreateWindow(WIDTH, HEIGHT, "Black hole sim", nullptr, nullptr);
    if (!window) {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    glEnable(GL_PROGRAM_POINT_SIZE);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed\n";
        return false;
    }
    while (glGetError() != GL_NO_ERROR) {}

    glfwSwapInterval(1);

    glEnable(GL_DEPTH_TEST);

    return true;
}


void Engine::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


void Engine::run() {
    processInput();
    render();
}


void Engine::render() {
    glClearColor(0.02f, 0.02f, 0.03f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (backgroundShader && backgroundTexture) {
        drawBackground();
    }
}


void Engine::cleanup() {
    glfwDestroyWindow(window);
    glfwTerminate();

}


std::string Engine::loadShaderFile(const std::string& shaderSource) {
    std::ifstream file(shaderSource);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << shaderSource << std::endl;
        return "";  // return empty string if file not found
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


GLuint Engine::CreateShaderProgram() {
    std::string fragmentShaderSource = loadShaderFile(this->fragmentShaderSourceFile);
    std::string vertexShaderSource = loadShaderFile(this->vertexShaderSourceFile);

    // --- Debug: print the sources being compiled ---
    std::cout << "\n--- Vertex Shader Source ---\n"
        << vertexShaderSource
        << "\n----------------------------\n";
    std::cout << "\n--- Fragment Shader Source ---\n"
        << fragmentShaderSource
        << "\n------------------------------\n";

    const GLchar* vertexSourcePtr = vertexShaderSource.c_str();
    const GLchar* fragmentSourcePtr = fragmentShaderSource.c_str();

    // --- Compile Vertex Shader ---
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSourcePtr, nullptr);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[1024];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed:\n" << infoLog << std::endl;
    }
    else {
        std::cout << "Vertex shader compiled successfully\n";
    }

    // --- Compile Fragment Shader ---
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSourcePtr, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
    }
    else {
        std::cout << "shader compiled successfully\n";
    }

    // --- Link Program ---
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
    }
    else {
        std::cout << "Shader program linked successfully\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}


GLuint Engine::CreateShaderProgram(const char* vertexPath, const char* fragmentPath) {
    std::string fragmentShaderSource = loadShaderFile(fragmentPath);
    std::string vertexShaderSource = loadShaderFile(vertexPath);

    // --- Debug: print the sources being compiled ---
    std::cout << "\n--- Vertex Shader Source ---\n"
        << vertexShaderSource
        << "\n----------------------------\n";
    std::cout << "\n--- Fragment Shader Source ---\n"
        << fragmentShaderSource
        << "\n------------------------------\n";

    const GLchar* vertexSourcePtr = vertexShaderSource.c_str();
    const GLchar* fragmentSourcePtr = fragmentShaderSource.c_str();

    // --- Compile Vertex Shader ---
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSourcePtr, nullptr);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[1024];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed:\n" << infoLog << std::endl;
    }
    else {
        std::cout << "Vertex shader compiled successfully\n";
    }

    // --- Compile Fragment Shader ---
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSourcePtr, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
    }
    else {
        std::cout << "shader compiled successfully\n";
    }

    // --- Link Program ---
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
    }
    else {
        std::cout << "Shader program linked successfully\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}


void Engine::loadBackground(const char* imagePath, const char* vertPath, const char* fragPath) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);

    if (!data) {
        std::cerr << "Failed to load background image: " << imagePath << std::endl;
    }
    else {
        std::cout << "Loaded background image: " << width << "x" << height
            << " channels=" << nrChannels << std::endl;
    }

    glGenTextures(1, &backgroundTexture);
    glBindTexture(GL_TEXTURE_3D, backgroundTexture);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_3D, 0, format, width, height, 0, format,
        GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_3D);

    stbi_image_free(data);

    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,   // top-left
        -1.0f, -1.0f,  0.0f, 0.0f,   // bottom-left
         1.0f, -1.0f,  1.0f, 0.0f,   // bottom-right

        -1.0f,  1.0f,  0.0f, 1.0f,   // top-left
         1.0f, -1.0f,  1.0f, 0.0f,   // bottom-right
         1.0f,  1.0f,  1.0f, 1.0f    // top-right
    };

    glGenVertexArrays(1, &backgroundVAO);
    glGenBuffers(1, &backgroundVBO);

    glBindVertexArray(backgroundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    backgroundShader = CreateShaderProgram(vertPath, fragPath);
}


void Engine::drawBackground() {
    glDisable(GL_DEPTH_TEST); 

    glUseProgram(backgroundShader);
    glBindVertexArray(backgroundVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, backgroundTexture);

    GLint texLoc = glGetUniformLocation(backgroundShader, "backgroundTexture");
    glUniform1i(texLoc, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}