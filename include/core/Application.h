#pragma once
#include "core/events/EventDispatcher.h"
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

// Forward declarations
class Window;
class Settings;
class Renderer;
class Scene;
class Event;
class KeyPressedEvent;

class Application {
public:
  Application();
  ~Application();

  // Initializes all components and starts the main render loop.
  void run();

private:
  void load_scripts();
  void subscribe_to_events();
  void on_key_pressed(KeyPressedEvent &event);
  void process_script_commands();

  std::unique_ptr<Window> m_window;
  std::unique_ptr<Settings> m_settings;
  std::unique_ptr<Renderer> m_renderer;
  std::unique_ptr<Scene> m_active_scene;

  std::vector<ScopedSubscription> m_subscriptions;

  std::thread m_console_thread;
  std::mutex m_command_mutex;
  std::vector<std::string> m_command_queue;
};
