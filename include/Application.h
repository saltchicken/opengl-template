#pragma once
#include <memory>

// Forward declarations
class Window;
class Settings;
class Renderer;
class Camera;

class Application {
public:
  Application();
  ~Application();

  // Initializes all components and starts the main render loop.
  void run();

private:
  std::unique_ptr<Window> m_window;
  std::unique_ptr<Settings> m_settings;
  std::unique_ptr<Renderer> m_renderer;
  std::unique_ptr<Camera> m_camera;
};
