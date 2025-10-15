#include "Application.h"
#include "Window.h"
#include "Renderer.h"
#include <iostream>

Application::Application() {
    m_window = std::make_unique<Window>();
    m_renderer = std::make_unique<Renderer>();
}

Application::~Application() = default;

void Application::run() {
    // 1. Initialize window and renderer
    if (!m_window->init(800, 600, "OpenGL Application")) {
        std::cerr << "Failed to initialize window!" << std::endl;
        return;
    }
    if (!m_renderer->init()) {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return;
    }

    // 2. Main loop
    while (!m_window->shouldClose()) {
        // Render
        m_renderer->draw();
        
        // Swap buffers and poll IO events
        m_window->swapBuffers();
        m_window->pollEvents();
    }
}
