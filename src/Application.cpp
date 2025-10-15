#include "Application.h"
#include "Window.h"
#include "Renderer.h"
#include "Input.h"
#include "Time.h"
#include <iostream>
#include <string>
#include <GLFW/glfw3.h>

Application::Application() {
    m_window = std::make_unique<Window>();
    m_renderer = std::make_unique<Renderer>();
    m_time = std::make_unique<Time>(60.0f); // TODO: Better way to determine and set target FPS
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
        m_time->beginFrame();


        // Input
        if (Input::is_key_pressed(GLFW_KEY_ESCAPE)) {
            m_window->setShouldClose(true);
        }

        // Render
        m_renderer->draw();
        
        // Swap buffers and poll IO events
        m_window->swapBuffers();
        m_window->pollEvents();

        m_time->endFrame();
    }
}
