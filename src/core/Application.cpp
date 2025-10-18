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
#include "scene/Scene.h"
#include "utils/Log.h"
#include "utils/ResourceManager.h"
#include "utils/ScriptingManager.h"

#include <GLFW/glfw3.h>

Application::Application() {
  m_window = std::make_unique<Window>();
  m_settings = std::make_unique<Settings>();
  m_renderer = std::make_unique<Renderer>();
  // Initialize camera looking at the origin from 3 units away
  m_active_scene = std::make_unique<Scene>();

  subscribe_to_events();
}

Application::~Application() {

  if (m_console_thread.joinable()) {
    // TODO: A real app might need a more graceful shutdown signal
    // but for this, we can detach as the app is closing.
    m_console_thread.detach();
  }
  ResourceManager::clear();
};

void Application::process_script_commands() {
  // Create a temporary copy of commands to process
  std::vector<std::string> commands_to_run;
  {
    // Lock the mutex to safely access the command queue
    std::lock_guard<std::mutex> lock(m_command_mutex);
    if (!m_command_queue.empty()) {
      commands_to_run.swap(m_command_queue);
    }
  } // Mutex is unlocked here

  // Execute commands outside the lock
  for (const auto &command : commands_to_run) {
    Log::info("Executing: " + command);
    ScriptingManager::execute_string(*m_active_scene, command);
  }
}

void Application::subscribe_to_events() {
  // CHANGE: Simply push the returned ScopedSubscription object into the vector.
  m_subscriptions.push_back(EventDispatcher::subscribe<KeyPressedEvent>(
      std::bind(&Application::on_key_pressed, this, std::placeholders::_1)));

  m_subscriptions.push_back(EventDispatcher::subscribe<KeyReleasedEvent>(
      [](KeyReleasedEvent &event) { Log::debug(event.to_string()); }));

  m_subscriptions.push_back(EventDispatcher::subscribe<WindowResizeEvent>(
      [](WindowResizeEvent &event) {
        Log::info("Window resized to: " + std::to_string(event.get_width()) +
                  "x" + std::to_string(event.get_height()));
      }));

  m_subscriptions.push_back(EventDispatcher::subscribe<MouseButtonPressedEvent>(
      [](MouseButtonPressedEvent &event) { Log::debug(event.to_string()); }));

  m_subscriptions.push_back(
      EventDispatcher::subscribe<MouseButtonReleasedEvent>(
          [](MouseButtonReleasedEvent &event) {
            Log::debug(event.to_string());
          }));
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

  ScriptingManager::init();

  m_console_thread = std::thread([this]() {
    Log::info(
        "Interactive Lua console started. Type commands and press Enter.");
    std::string line;
    while (std::getline(std::cin, line)) {
      if (!line.empty()) {
        // Lock the mutex to safely push the command to the queue
        std::lock_guard<std::mutex> lock(m_command_mutex);
        m_command_queue.push_back(line);
      }
    }
  });

  //
  // SCENE SETUP
  //
  if (!ScriptingManager::load_scene_script(*m_active_scene,
                                           "scripts/scene.lua")) {
    Log::error("Could not build scene from script. Aborting.");
    return;
  }
  //
  // End SCENE SETUP
  //

  Input *input = m_window->get_input();
  Log::debug("Starting main loop");
  while (!m_window->should_close()) {
    Time::begin_frame();

    double delta_time = Time::get_delta_time();

    process_script_commands();
    m_window->poll_events();

    EventDispatcher::dispatch_events();
    if (m_window->should_close()) {
      continue;
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
