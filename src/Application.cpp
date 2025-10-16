#include "Application.h"
#include "Camera.h"
#include "Input.h"
#include "Renderer.h"
#include "Time.h"
#include "Window.h"
#include <GLFW/glfw3.h>
#include <iostream>

Application::Application() {
  m_window = std::make_unique<Window>();
  m_renderer = std::make_unique<Renderer>();
  // Initialize camera looking at the origin from 3 units away
  m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 1.0f));
  Time::init(60.0f); // TODO: Better way to determine and set target FPS
}

Application::~Application() = default;

void Application::run() {
  // 1. Initialize window and renderer
  if (!m_window->init(800, 600, "OpenGL Application")) {
    std::cerr << "Failed to initialize window!" << std::endl;
    return;
  }
  if (!m_renderer->init()) {
    std::cerr << "Failed to initialize renderer!" << std::endl;
    return;
  }

  Input *input = m_window->get_input();

  // 2. Main loop
  while (!m_window->should_close()) {
    Time::begin_frame();

    m_window->poll_events();
    if (!input->process_input(*m_window)) {
      break;
    }

    // Render
    m_renderer->update(Time::get_delta_time());
    m_renderer->draw(*m_camera, m_window->get_width(), m_window->get_height());
    m_window->swap_buffers();
    input->update();
    Time::end_frame();
  }
}
