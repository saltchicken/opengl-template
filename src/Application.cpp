#include "Application.h"
#include "Window.h"
#include "Renderer.h"
#include "Input.h"
#include <iostream>
#include <string>
#include <GLFW/glfw3.h> // Include for glfwGetTime()

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
        // TODO: Move FPS logic into a separate function
        float currentFrame = static_cast<float>(glfwGetTime());
        m_deltaTime = currentFrame - m_lastFrame;
        m_lastFrame = currentFrame;

        // --- FPS Counter (optional, but useful) ---
        // Display FPS in the window title every second
        static double lastTime = glfwGetTime();
        static int nbFrames = 0;
        nbFrames++;
        if (currentFrame - lastTime >= 1.0) {
            // TODO: Better way to display FPS or remove this when FPS limitting is implemented
            std::cout << "FPS: " << nbFrames << std::endl;
            nbFrames = 0;
            lastTime += 1.0;
        }

        // Input
        if (Input::is_key_pressed(GLFW_KEY_ESCAPE)) {
            m_window->setShouldClose(true);
        }

        // Render
        m_renderer->draw();
        
        // Swap buffers and poll IO events
        m_window->swapBuffers();
        m_window->pollEvents();
    }
}
