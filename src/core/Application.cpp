#include "core/Application.h"
#include "core/Input.h"
#include "core/ScriptingContext.h"
#include "core/Settings.h"
#include "core/Time.h"
#include "core/Window.h"
#include "core/events/AppEvent.h"
#include "core/events/EventDispatcher.h"
#include "core/events/KeyEvent.h"
#include "core/events/MouseEvent.h"
#include "graphics/CanvasRenderer.h"
#include "graphics/IRenderer.h"
#include "graphics/SceneRenderer.h"
#include "scene/Scene.h"
#include "utils/DebugConsole.h"
#include "utils/Log.h"
#include "utils/ResourceManager.h"
#include "utils/ScriptingManager.h"

#include <GLFW/glfw3.h>

Application::Application() {
  m_window = std::make_unique<Window>();
  m_settings = std::make_unique<Settings>();
  m_active_scene = std::make_unique<Scene>();
  m_scripting_context = std::make_unique<ScriptingContext>();
  m_console = std::make_unique<DebugConsole>();

  subscribe_to_events();
}

Application::~Application() { ResourceManager::clear(); };

void Application::load_scripts() {
  Log::info("--- Loading Scripts ---");
  // Load runtime settings from Lua, which can load resources and set parameters
  if (!ScriptingManager::load_runtime_settings(
          *m_settings, "scripts/runtime_settings.lua")) {
    Log::warn("Could not load runtime settings from script. Using defaults.");
  }

  // Load the scene from its dedicated script
  if (!ScriptingManager::load_scene_script(*m_active_scene,
                                           "scripts/empty.lua")) {
    Log::error("FATAL: Could not build scene from script.");
  }
  Log::info("--- Script Loading Complete ---");
}

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
    // Log::info("Executing: " + command);
    m_console->add_log("Executing: " + command);
    ScriptingManager::run_command(command);
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
  m_console->on_key_pressed(event.get_key_code());
  if (event.get_key_code() == GLFW_KEY_ESCAPE) {
    m_window->set_should_close(true);
  }
}

void Application::run() {
  // 1. Load critical settings TOML first.
  m_settings->load("settings.toml");
  const auto &config = m_settings->get_config();

  // 2. Initialize the window and OpenGL context
  if (!m_window->init(config.window_width, config.window_height,
                      config.window_title.c_str(), config.window_resizable,
                      config.window_transparent)) {
    Log::error("Failed to initialize window!");
    return;
  }
  m_console->init(
      m_window->get_glfw_window()); // You'll need to add this getter
  m_console->set_command_callback([this](const std::string &command) {
    std::lock_guard<std::mutex> lock(m_command_mutex);
    m_command_queue.push_back(command);
  });

  // 3. Initialize scripting and load assets.
  ScriptingManager::init();
  m_scripting_context->scene = m_active_scene.get();

  ScriptingManager::set_context(*m_scripting_context);
  load_scripts();

  // 5. Initialize the renderer, which depends on loaded shaders.
  if (config.renderer_type == "canvas") {
    Log::info("Creating CanvasRenderer based on settings.");
    m_renderer = std::make_unique<CanvasRenderer>();
  } else if (config.renderer_type == "scene") {
    Log::info("Creating SceneRenderer based on settings.");
    m_renderer = std::make_unique<SceneRenderer>();
  } else {
    Log::warn("Unknown renderer type '" + config.renderer_type +
              "'. Defaulting to SceneRenderer.");
    m_renderer = std::make_unique<SceneRenderer>();
  }

  m_scripting_context->renderer = m_renderer.get();
  // 5. Initialize the chosen renderer.
  if (!m_renderer->init(config)) {
    Log::error("Failed to initialize renderer!");
    return;
  }

  // 6. Initialize time.
  Time::init(config.fps);

  // --- MAIN LOOP ---
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
    m_console->draw();

    m_window->swap_buffers();
    input->update();
    Time::end_frame();
  }
}
