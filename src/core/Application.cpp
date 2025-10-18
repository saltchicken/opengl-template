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
#include "scene/IfsFractalComponent.h"
#include "scene/PropertyAnimatorComponent.h"
#include "scene/Scene.h"
#include "scene/SceneObject.h"
#include "utils/Log.h"
#include "utils/ResourceManager.h"
#include <GLFW/glfw3.h>
#include <numbers>

Application::Application() {
  m_window = std::make_unique<Window>();
  m_settings = std::make_unique<Settings>();
  m_renderer = std::make_unique<Renderer>();
  m_active_scene = std::make_unique<Scene>();

  subscribe_to_events();
}

Application::~Application() { ResourceManager::clear(); };

void Application::subscribe_to_events() {
  m_subscriptions.push_back(EventDispatcher::subscribe<KeyPressedEvent>(
      std::bind(&Application::on_key_pressed, this, std::placeholders::_1)));

  m_subscriptions.push_back(EventDispatcher::subscribe<WindowResizeEvent>(
      std::bind(&Application::on_window_resize, this, std::placeholders::_1)));
}

void Application::on_key_pressed(KeyPressedEvent &event) {
  Log::debug(event.to_string());
  if (event.get_key_code() == GLFW_KEY_ESCAPE) {
    m_window->set_should_close(true);
  }
}

void Application::on_window_resize(WindowResizeEvent &event) {
  Log::info("Window resized to: " + std::to_string(event.get_width()) + "x" +
            std::to_string(event.get_height()));
  if (m_renderer) {
    m_renderer->on_window_resize(event.get_width(), event.get_height());
  }
}

void Application::run() {
  m_settings->load("settings.toml");
  const auto &config = m_settings->get_config();

  Time::init(config.fps);

  if (!m_window->init(config.window_width, config.window_height,
                      config.window_title.c_str(), config.window_resizable,
                      config.window_transparent)) {
    Log::error("Failed to initialize window!");
    return;
  }

  if (!m_renderer->init(config, m_window->get_width(),
                        m_window->get_height())) {
    Log::error("Failed to initialize renderer!");
    return;
  }

  //
  // SCENE SETUP
  //

  auto camera_object = std::make_shared<SceneObject>(nullptr);
  camera_object->transform->position = glm::vec3(0.0f, 0.0f, 60.0f);
  camera_object->add_component<CameraComponent>(45.0f, 2.0f, 100.0f);
  m_active_scene->add_object(camera_object);
  m_active_scene->set_active_camera(camera_object);

  auto point_cloud_mesh =
      std::make_shared<Mesh>(std::vector<Vertex>{}, std::vector<unsigned int>{},
                             std::vector<std::shared_ptr<Texture>>{});
  auto fractal_object = std::make_shared<SceneObject>(point_cloud_mesh);
  fractal_object->transform->scale = glm::vec3(3.0f);
  fractal_object->add_component<IfsFractalComponent>(
      200000, 150, config.fractal_denoise_factor);
  fractal_object->add_component<PropertyAnimatorComponent>(
      PropertyAnimatorComponent::TargetProperty::ROTATION,
      PropertyAnimatorComponent::RotationParams{glm::vec3(0.0f, 1.0f, 0.0f),
                                                15.0f});

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
    EventDispatcher::dispatch_events();

    if (m_window->should_close()) {
      continue;
    }

    m_active_scene->update(delta_time);

    m_renderer->draw(*m_active_scene, config, m_window->get_width(),
                     m_window->get_height());

    m_window->swap_buffers();
    input->update();

    Time::end_frame();
  }
}
