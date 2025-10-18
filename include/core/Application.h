#pragma once
#include "core/events/EventDispatcher.h" // Include for the handle type
#include <memory>
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
  void subscribe_to_events();
  void on_key_pressed(KeyPressedEvent &event);

  std::unique_ptr<Window> m_window;
  std::unique_ptr<Settings> m_settings;
  std::unique_ptr<Renderer> m_renderer;
  std::unique_ptr<Scene> m_active_scene;

  std::vector<ScopedSubscription> m_subscriptions;
};
