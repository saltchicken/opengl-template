#include "core/Application.h"
#include "core/Input.h"
#include "core/Settings.h"
#include "core/Time.h"
#include "core/Window.h"
#include "graphics/Renderer.h"
#include "scene/RotationComponent.h"
#include "scene/Scene.h"
#include "scene/SceneObject.h"
#include "utils/Log.h"
#include "utils/ResourceManager.h"

#include <GLFW/glfw3.h>

Application::Application() {
  m_window = std::make_unique<Window>();
  m_settings = std::make_unique<Settings>();
  m_renderer = std::make_unique<Renderer>();
  // Initialize camera looking at the origin from 3 units away
  m_active_scene = std::make_unique<Scene>();
}

Application::~Application() { ResourceManager::clear(); };

void Application::run() {
  m_settings->load("settings.toml");
  const auto &config = m_settings->get_config();

  Time::init(config.fps);
  // 1. Initialize window and renderer
  if (!m_window->init(config.window_width, config.window_height,
                      config.window_title.c_str(), config.window_resizable)) {
    Log::error("Failed to initialize window!");
    return;
  }
  if (!m_renderer->init()) {
    Log::error("Failed to initialize renderer!");
    return;
  }

  // Create a quad mesh
  auto cube_mesh = ResourceManager::get_primitive("cube");

  auto my_texture =
      ResourceManager::load_texture("test", "assets/textures/test.png");
  if (my_texture) {
    cube_mesh->textures.push_back(my_texture);
  }
  // 2. Create a scene object using the mesh.
  auto my_object = std::make_shared<SceneObject>(cube_mesh);
  my_object->add_component<RotationComponent>(
      glm::normalize(glm::vec3(0.5f, 1.0f, 0.0f)), 50.0f);
  // You can set its position, rotation, or scale here if you want.
  // For example, to move it right:
  // my_object->transform = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f,
  // 0.0f, 0.0f));

  // 3. Submit the object to the renderer.
  m_active_scene->add_object(my_object);

  Input *input = m_window->get_input();
  Log::debug("Starting main loop");
  while (!m_window->should_close()) {
    Time::begin_frame();

    m_window->poll_events();

    // NOTE: This is required to prevent SIGSEGV (Address boundary error)
    if (!input->process_input(*m_window)) {
      break;
    }

    // Update all object and their components in the scene
    m_active_scene->update(Time::get_delta_time());

    // Render
    m_renderer->update(Time::get_delta_time());
    m_renderer->draw(*m_active_scene, m_window->get_width(),
                     m_window->get_height());
    m_window->swap_buffers();
    input->update();
    Time::end_frame();
  }
}
