#include "Application.h"
#include "Input.h"
#include "Renderer.h"
#include "Time.h"
#include "Window.h"
#include <GLFW/glfw3.h>
#include <iostream>

Application::Application() {
  m_window = std::make_unique<Window>();
  m_renderer = std::make_unique<Renderer>();
  m_time = std::make_unique<Time>(
      60.0f); // TODO: Better way to determine and set target FPS
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
    m_time->begin_frame();

    m_window->poll_events();

    input->process_input(*m_window);

    // TODO: This needs to run after process_input or else it causes a SIGSEGV
    // (Address boundary error). Need a cleaner way to handle this.
    if (m_window->should_close()) {
      break;
    }

    // Render
    m_renderer->draw();

    // Swap buffers and poll IO events
    m_window->swap_buffers();

    input->update();

    m_time->end_frame();
  }
}
