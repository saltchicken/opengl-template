#include "Application.h"
#include "Camera.h"
#include "Input.h"
#include "PrimitiveFactory.h"
#include "Renderer.h"
#include "SceneObject.h"
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

  // Create a quad mesh
  auto quad_mesh = PrimitiveFactory::create_quad();

  // 2. Create a scene object using the mesh.
  auto my_object = std::make_shared<SceneObject>(quad_mesh);
  // You can set its position, rotation, or scale here if you want.
  // For example, to move it right:
  // my_object->transform = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f,
  // 0.0f, 0.0f));

  // 3. Submit the object to the renderer.
  m_renderer->submit(my_object);

  Input *input = m_window->get_input();
  while (!m_window->should_close()) {
    Time::begin_frame();

    m_window->poll_events();

    // NOTE: This is required to prevent SIGSEGV (Address boundary error)
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
