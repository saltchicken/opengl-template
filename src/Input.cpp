#include "Input.h"
#include "Window.h"
#include <cstring>
#include <iostream> // For example usage

Input::Input(GLFWwindow *window) : m_window(window) {}

void Input::update() {
  memcpy(m_previous_key_states, m_current_key_states,
         sizeof(m_current_key_states));
}

void Input::process_input(Window &window) {
  if (is_key_down(GLFW_KEY_ESCAPE)) {
    window.set_should_close(true);
  }
  if (is_key_released(GLFW_KEY_SPACE)) {
    std::cout << "Spacebar released!" << std::endl;
  }
  // Add other input handling here
}

// Static callback that GLFW uses
void Input::key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  // Retrieve the Input instance from the window's user pointer
  Input *input_handler = static_cast<Input *>(glfwGetWindowUserPointer(window));
  if (input_handler) {
    input_handler->key_callback_impl(key, action);
  }
}

// Instance-specific implementation of the callback logic
void Input::key_callback_impl(int key, int action) {
  if (key >= 0 && key < 1024) {
    if (action == GLFW_PRESS) {
      m_current_key_states[key] = true;
    } else if (action == GLFW_RELEASE) {
      m_current_key_states[key] = false;
    }
  }
}

bool Input::is_key_down(int key) const {
  if (key >= 0 && key < 1024) {
    return m_current_key_states[key];
  }
  return false;
}

bool Input::is_key_pressed(int key) const {
  if (key >= 0 && key < 1024) {
    // Was the key up last frame, but is down this frame?
    return m_current_key_states[key] && !m_previous_key_states[key];
  }
  return false;
}

bool Input::is_key_released(int key) const {
  if (key >= 0 && key < 1024) {
    // Was the key down last frame, but is up this frame?
    return !m_current_key_states[key] && m_previous_key_states[key];
  }
  return false;
}
