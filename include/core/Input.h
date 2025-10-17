#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Window;

class Input {
public:
  Input(GLFWwindow *window);

  void update();

  bool process_input(Window &window);

  static void key_callback(GLFWwindow *window, int key, int scancode,
                           int action, int mods);

  bool is_key_down(int key) const;
  bool is_key_pressed(int key) const;
  bool is_key_released(int key) const;

private:
  void key_callback_impl(int key, int action);

  bool m_current_key_states[GLFW_KEY_LAST + 1] = {false};
  bool m_previous_key_states[GLFW_KEY_LAST + 1] = {false};
  GLFWwindow *m_window; // Keep a pointer to the window
};
