#pragma once

#include <functional>
#include <string>
#include <vector>

struct GLFWwindow;

class DebugConsole {
public:
  DebugConsole();
  ~DebugConsole();

  // Callbacks for application events
  void init(GLFWwindow *window);
  void on_key_pressed(int key);

  // Main functions
  void draw();
  void add_log(const std::string &log);
  void set_command_callback(
      const std::function<void(const std::string &)> &callback);

private:
  void execute_command(const std::string &command);

  char m_input_buffer[256];
  std::vector<std::string> m_log_history;
  std::vector<std::string> m_command_history;
  int m_history_pos;
  bool m_is_visible;
  std::function<void(const std::string &)> m_command_callback;
};
