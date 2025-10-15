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
    void updateTimeAndFPS();
    
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;

    const float m_targetFPS = 60.0f;
    float m_deltaTime = 0.0f;
    float m_lastFrame = 0.0f;

    double m_lastFPSTime = 0.0;
    int m_frameCount = 0;
};
