#include "Input.h"
#include "Window.h"

Input::Input(GLFWwindow* window) : m_window(window) {}

void Input::processInput(Window& window) {
    if (is_key_pressed(GLFW_KEY_ESCAPE)) {
        window.setShouldClose(true);
    }
    // Add other input handling here
}

// Static callback that GLFW uses
void Input::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Retrieve the Input instance from the window's user pointer
    Input* input_handler = static_cast<Input*>(glfwGetWindowUserPointer(window));
    if (input_handler) {
        input_handler->key_callback_impl(key, action);
    }
}

// Instance-specific implementation of the callback logic
void Input::key_callback_impl(int key, int action) {
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            keys[key] = true;
        } else if (action == GLFW_RELEASE) {
            keys[key] = false;
        }
    }
}

bool Input::is_key_pressed(int key) {
    if (key >= 0 && key < 1024) {
        return keys[key];
    }
    return false;
}
