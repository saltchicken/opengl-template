#include "core/Application.h"
#include "core/Input.h"
#include "core/Settings.h"
#include "core/Time.h"
#include "core/Window.h"
#include "graphics/Renderer.h"
#include "scene/CameraComponent.h"
#include "scene/FractalComponent.h"
#include "scene/PhyllotaxisComponent.h"
#include "scene/PropertyAnimatorComponent.h"
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
                      config.window_title.c_str(), config.window_resizable,
                      config.window_transparent)) {
    Log::error("Failed to initialize window!");
    return;
  }
  if (!m_renderer->init(config.window_transparent)) {
    Log::error("Failed to initialize renderer!");
    return;
  }

  //
  // SCENE SETUP
  // TODO: This needs to be factored out of application.cpp

  auto camera_object = std::make_shared<SceneObject>(nullptr);
  camera_object->transform->position = glm::vec3(0.0f, 0.0f, 20.0f);
  camera_object->add_component<CameraComponent>(45.0f, 0.1f, 100.0f);
  m_active_scene->add_object(camera_object);
  m_active_scene->set_active_camera(camera_object);

  // --- REPLACE THE SINGLE SPHERE WITH THE FRACTAL OBJECT ---

  // 1. Load resources
  auto my_texture =
      ResourceManager::load_texture("test", "assets/textures/test2.png");
  auto phyllo_mesh = ResourceManager::create_primitive_instance("sphere");
  if (my_texture) {
    phyllo_mesh->textures.push_back(my_texture);
  }

  // // 2. Create the host SceneObject
  auto pattern_object = std::make_shared<SceneObject>(phyllo_mesh);

  // 2. REPLACE FractalComponent with PhyllotaxisComponent
  // Let's create a shape with 8000 points and a radius of 4 units
  pattern_object->add_component<PhyllotaxisComponent>(8000, 4.0f);

  // Keep the animator to make the new shape rotate
  pattern_object->add_component<PropertyAnimatorComponent>(
      PropertyAnimatorComponent::TargetProperty::ROTATION,
      PropertyAnimatorComponent::RotationParams{glm::vec3(0.5f, 1.0f, 0.0f),
                                                15.0f});

  // Add the single host object to the scene
  m_active_scene->add_object(pattern_object);

  auto fractal_mesh = ResourceManager::create_primitive_instance("sphere");
  if (my_texture) {
    fractal_mesh->textures.push_back(my_texture);
  }

  auto fractal_object = std::make_shared<SceneObject>(fractal_mesh);

  fractal_object->transform->scale = glm::vec3(2.5f);
  // 3. Add the FractalComponent (depth 3 = 20^3 = 8000 spheres)
  fractal_object->add_component<FractalComponent>(3, 0.50f);

  // 4. (Optional) Add an animator to make the whole fractal rotate
  fractal_object->add_component<PropertyAnimatorComponent>(
      PropertyAnimatorComponent::TargetProperty::ROTATION,
      PropertyAnimatorComponent::RotationParams{glm::vec3(0.5f, 1.0f, 0.0f),
                                                10.0f});

  // 5. Add the single host object to the scene
  m_active_scene->add_object(fractal_object);

  //
  // End SCENE SETUP
  //

  Input *input = m_window->get_input();
  Log::debug("Starting main loop");
  while (!m_window->should_close()) {
    Time::begin_frame();

    double delta_time = Time::get_delta_time();

    m_window->poll_events();

    // NOTE: This is required to prevent SIGSEGV (Address boundary error)
    if (!input->process_input(*m_window)) {
      break;
    }

    // Update all object and their components in the scene
    m_active_scene->update(delta_time);

    // Render
    m_renderer->update(delta_time);
    m_renderer->draw(*m_active_scene, m_window->get_width(),
                     m_window->get_height());
    m_window->swap_buffers();
    input->update();
    Time::end_frame();
  }
}
