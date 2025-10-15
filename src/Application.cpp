#include "Application.h"
#include "Window.h"
#include "Renderer.h"
#include "Input.h"
#include <iostream>
#include <string>
#include <GLFW/glfw3.h>
#include <thread>
#include <chrono>

Application::Application() {
    m_window = std::make_unique<Window>();
    m_renderer = std::make_unique<Renderer>();
}

Application::~Application() = default;

void Application::updateTimeAndFPS() {
    double currentFrame = glfwGetTime();
    m_deltaTime = static_cast<float>(currentFrame - m_lastFrame);
    m_lastFrame = static_cast<float>(currentFrame);

    // FPS Counter Logic
    m_frameCount++;
    // If one second has passed
    if (currentFrame - m_lastFPSTime >= 1.0) {
        // TODO: Better way to display FPS
        std::cout << "FPS: " << m_frameCount << std::endl;
        // Reset for the next second
        m_frameCount = 0;
        m_lastFPSTime = currentFrame;
    }
}

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

    const float targetFrameTime = 1.0f / m_targetFPS;

    // 2. Main loop
    while (!m_window->shouldClose()) {
        updateTimeAndFPS();
        double frameStartTime = glfwGetTime();


        // Input
        if (Input::is_key_pressed(GLFW_KEY_ESCAPE)) {
            m_window->setShouldClose(true);
        }

        // Render
        m_renderer->draw();
        
        // Swap buffers and poll IO events
        m_window->swapBuffers();
        m_window->pollEvents();

        double elapsedTime = glfwGetTime() - frameStartTime;

        if (elapsedTime < targetFrameTime) {
            // Calculate sleep duration based on the work of the CURRENT frame
            auto sleepDuration = std::chrono::duration<double>(targetFrameTime - elapsedTime);
            std::this_thread::sleep_for(sleepDuration);
        }
    }
}
