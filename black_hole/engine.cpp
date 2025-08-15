#include "engine.h"

#include <iostream>


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
}


void Engine::cleanup() {
    glfwDestroyWindow(window);
    glfwTerminate();

}


std::string Engine::loadShaderFile(const std::string& shaderSource){
    std::string result = "";
    std::string line = "";
    std::ifstream file(shaderSource.c_str());

    if (file.is_open()){
        while(std::getline(file, line)){
            result += line + '\n';
        }

        file.close();
    }

    return result;
}


GLuint Engine::CreateShaderProgram(){
    std::string fragmentShaderSource = loadShaderFile(this->fragmentShaderSourceFile);
    std::string vertexShaderSource   = loadShaderFile(this->vertexShaderSourceFile);

    //std::cout << "fragment shader" << fragmentShaderSource << std::endl;
    //std::cout << "vertex shader" << vertexShaderSource << std::endl;
    
    const GLchar* vertexSourcePtr   = vertexShaderSource.c_str();
    const GLchar* fragmentSourcePtr = fragmentShaderSource.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSourcePtr, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSourcePtr, nullptr);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}
