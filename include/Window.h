#pragma once
#include <memory>

// Forward declare GLFWwindow to avoid including glfw3.h in the header
struct GLFWwindow;
class Input;

class Window {
public:
  Window();
  ~Window();

  // Initializes GLFW, creates a window, and initializes GLAD.
  bool init(unsigned int width, unsigned int height, const char *title);

  // Checks if the window has been flagged to close.
  bool should_close();
  void set_should_close(bool value);

  // Swaps the front and back buffers.
  void swap_buffers();

  // Processes all pending events.
  void poll_events();

  // Getter to access the input handler from the Application class
  Input *get_input();

private:
  GLFWwindow *m_window = nullptr;
  std::unique_ptr<Input>
      m_input_handler; // The window now owns the input handler
};
