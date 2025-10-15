#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Window;

class Input {
public:
  Input(GLFWwindow *window);

  void process_input(Window &window);

  static void key_callback(GLFWwindow *window, int key, int scancode,
                           int action, int mods);

  bool is_key_pressed(int key);

private:
  void key_callback_impl(int key, int action);

  bool keys[1024] = {false};
  GLFWwindow *m_window; // Keep a pointer to the window
};
