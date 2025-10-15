#pragma once

// Forward declare GLFWwindow to avoid including glfw3.h in the header
struct GLFWwindow;

class Window {
public:
    Window();
    ~Window();

    // Initializes GLFW, creates a window, and initializes GLAD.
    bool init(unsigned int width, unsigned int height, const char* title);
    
    // Checks if the window has been flagged to close.
    bool shouldClose();
    void setShouldClose(bool value);

    // Swaps the front and back buffers.
    void swapBuffers();

    // Processes all pending events.
    void pollEvents();

private:
    GLFWwindow* m_window = nullptr;
};
