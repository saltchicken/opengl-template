#pragma once
#include <memory>

// Forward declarations
class Window;
class Renderer;

class Application {
public:
    Application();
    ~Application();

    // Initializes all components and starts the main render loop.
    void run();

private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;

    float m_deltaTime = 0.0f;
    float m_lastFrame = 0.0f;
};
