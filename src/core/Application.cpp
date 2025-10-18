#include "core/Application.h"
#include "core/Input.h"
#include "core/Settings.h"
#include "core/Time.h"
#include "core/Window.h"
#include "core/events/AppEvent.h"
#include "core/events/EventDispatcher.h"
#include "core/events/KeyEvent.h"
#include "core/events/MouseEvent.h"
#include "graphics/Renderer.h"
#include "scene/CameraComponent.h"
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

  subscribe_to_events();
}

Application::~Application() { ResourceManager::clear(); };

void Application::subscribe_to_events() {
  // Subscribe the on_key_pressed member function to KeyPressedEvent
  EventDispatcher::subscribe<KeyPressedEvent>(
      std::bind(&Application::on_key_pressed, this, std::placeholders::_1));

  EventDispatcher::subscribe<KeyReleasedEvent>([](KeyReleasedEvent &event) {
    Log::debug(event.to_string()); // This will print the debug message
  });

  // You can also subscribe with a lambda for simple things
  EventDispatcher::subscribe<WindowResizeEvent>([](WindowResizeEvent &event) {
    Log::info("Window resized to: " + std::to_string(event.get_width()) + "x" +
              std::to_string(event.get_height()));
  });

  EventDispatcher::subscribe<MouseButtonPressedEvent>(
      [](MouseButtonPressedEvent &event) { Log::debug(event.to_string()); });

  EventDispatcher::subscribe<MouseButtonReleasedEvent>(
      [](MouseButtonReleasedEvent &event) { Log::debug(event.to_string()); });
}

void Application::on_key_pressed(KeyPressedEvent &event) {
  Log::debug(event.to_string());
  if (event.get_key_code() == GLFW_KEY_ESCAPE) {
    m_window->set_should_close(true);
  }
}

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

  auto camera_object = std::make_shared<SceneObject>(nullptr); // No mesh needed
  camera_object->transform->position =
      glm::vec3(0.0f, 0.0f, 5.0f); // Position it
  camera_object->add_component<CameraComponent>(45.0f, 0.1f, 100.0f);

  // 2. Add it to the scene and set it as the active camera
  m_active_scene->add_object(camera_object);
  m_active_scene->set_active_camera(camera_object);

  auto cube_mesh = ResourceManager::get_primitive("cube");
  auto my_texture =
      ResourceManager::load_texture("test", "assets/textures/test.png");
  if (my_texture) {
    cube_mesh->textures.push_back(my_texture);
  }

  // Create a quad mesh
  auto rotating_object = std::make_shared<SceneObject>(cube_mesh);
  rotating_object->transform->position = glm::vec3(-1.5f, 0.0f, 0.0f);

  // Use the new component to add rotation behavior
  rotating_object->add_component<PropertyAnimatorComponent>(
      PropertyAnimatorComponent::TargetProperty::ROTATION,
      PropertyAnimatorComponent::RotationParams{
          glm::normalize(glm::vec3(0.5f, 1.0f, 0.0f)), 50.0f});

  m_active_scene->add_object(rotating_object);

  // === Object 2: The Bobbing Cube ===
  auto bobbing_object = std::make_shared<SceneObject>(cube_mesh);
  bobbing_object->transform->position = glm::vec3(1.5f, 0.0f, 0.0f);

  // Use the same component to add position behavior (bobbing up and down)
  bobbing_object->add_component<PropertyAnimatorComponent>(
      PropertyAnimatorComponent::TargetProperty::POSITION,
      PropertyAnimatorComponent::PositionParams{glm::vec3(0.0f, 1.0f, 0.0f),
                                                2.0f, 0.5f});

  m_active_scene->add_object(bobbing_object);

  auto multi_anim_object = std::make_shared<SceneObject>(cube_mesh);
  multi_anim_object->transform->position =
      glm::vec3(0.0f, -1.5f, 0.0f); // Start it lower down

  // 1. Add the rotation behavior
  multi_anim_object->add_component<PropertyAnimatorComponent>(
      PropertyAnimatorComponent::TargetProperty::ROTATION,
      PropertyAnimatorComponent::RotationParams{glm::vec3(0.0f, 1.0f, 0.0f),
                                                80.0f});

  // 2. Add the position behavior TO THE SAME OBJECT
  multi_anim_object->add_component<PropertyAnimatorComponent>(
      PropertyAnimatorComponent::TargetProperty::POSITION,
      PropertyAnimatorComponent::PositionParams{glm::vec3(0.0f, 1.0f, 0.0f),
                                                1.5f, 0.7f});

  m_active_scene->add_object(multi_anim_object);

  auto sphere_mesh = ResourceManager::get_primitive("sphere");
  if (my_texture) {
    sphere_mesh->textures.push_back(my_texture);
  }
  auto sphere_object = std::make_shared<SceneObject>(sphere_mesh);
  sphere_object->transform->position = glm::vec3(0.0f, 1.5f, 0.0f);
  m_active_scene->add_object(sphere_object);
  //
  // End SCENE SETUP
  //

  Input *input = m_window->get_input();
  Log::debug("Starting main loop");
  while (!m_window->should_close()) {
    Time::begin_frame();

    double delta_time = Time::get_delta_time();

    m_window->poll_events();

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
