#include "Window.h"
#include "Input.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Callback for window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

Window::Window() = default;

Window::~Window() {
    if (m_window) {
        glfwDestroyWindow(m_window);
    }
    glfwTerminate();
    std::cout << "Window destroyed and GLFW terminated." << std::endl;
}

bool Window::init(unsigned int width, unsigned int height, const char* title) {
    // 1. Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2. Create Window
    m_window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    // Set the user pointer of the window to our input handler instance
    m_input_handler = std::make_unique<Input>(m_window);
    glfwSetWindowUserPointer(m_window, m_input_handler.get());

    glfwMakeContextCurrent(m_window);

    glfwSwapInterval(0); // Disable vsync

    glfwSetKeyCallback(m_window, Input::key_callback);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    // 3. Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    
    std::cout << "Window and OpenGL context initialized successfully." << std::endl;
    return true;
}

Input* Window::getInput() {
    return m_input_handler.get();
}

bool Window::shouldClose() {
  return glfwWindowShouldClose(m_window);
}

void Window::setShouldClose(bool value) {
    glfwSetWindowShouldClose(m_window, value);
}

void Window::swapBuffers() {
    glfwSwapBuffers(m_window);
}

void Window::pollEvents() {
    glfwPollEvents();
}
