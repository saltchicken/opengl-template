#include "core/Input.h"
#include "core/Window.h"
#include "core/events/EventDispatcher.h"
#include "core/events/KeyEvent.h"
#include <cstring>

Input::Input(GLFWwindow *window) : m_window(window) {}

void Input::update() {
  memcpy(m_previous_key_states, m_current_key_states,
         sizeof(m_current_key_states));
}

// Static callback that GLFW uses
void Input::key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (action == GLFW_PRESS) {
    KeyPressedEvent event(key);
    EventDispatcher::publish(event);
  } else if (action == GLFW_RELEASE) {
    KeyReleasedEvent event(key);
    EventDispatcher::publish(event);
  }

  // The rest of this function can remain to update the internal state for
  // is_key_down() polling, as both systems can coexist.

  // 1. Retrieve the Window instance from the user pointer
  Window *window_instance =
      static_cast<Window *>(glfwGetWindowUserPointer(window));

  // 2. Get the Input handler from the Window instance
  if (window_instance) {
    Input *input_handler = window_instance->get_input();
    if (input_handler) {
      input_handler->key_callback_impl(key, action);
    }
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
