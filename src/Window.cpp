#include "Window.h"
#include "Input.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

Window::Window() = default;

Window::~Window() {
  if (m_window) {
    glfwDestroyWindow(m_window);
  }
  glfwTerminate();
  std::cout << "Window destroyed and GLFW terminated." << std::endl;
}

bool Window::init(unsigned int width, unsigned int height, const char *title,
                  bool resizable) {
  m_width = width;
  m_height = height;
  // 1. Initialize GLFW
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return false;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);

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

  glViewport(0, 0, m_width, m_height);

  std::cout << "Window and OpenGL context initialized successfully."
            << std::endl;
  return true;
}

Input *Window::get_input() { return m_input_handler.get(); }

bool Window::should_close() { return glfwWindowShouldClose(m_window); }

void Window::set_should_close(bool value) {
  glfwSetWindowShouldClose(m_window, value);
}

void Window::swap_buffers() { glfwSwapBuffers(m_window); }

void Window::poll_events() { glfwPollEvents(); }

void Window::framebuffer_size_callback(GLFWwindow *window, int width,
                                       int height) {
  glViewport(0, 0, width, height);
  // We need to update our window's size fields, but we can't access
  // 'this' directly. One way is to get the app/window instance from the user
  // pointer if we had set it to the Window instance. For now, we'll just
  // handle the viewport. For a more robust solution, you'd retrieve your
  // Window instance from the user pointer and update its members.
}
