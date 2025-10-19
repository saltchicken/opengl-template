#include "utils/DebugConsole.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <utility>

int DebugConsole::text_edit_callback(ImGuiInputTextCallbackData *data) {
  // data->UserData contains the 'this' pointer we passed to InputText()
  DebugConsole *console = static_cast<DebugConsole *>(data->UserData);

  if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory) {
    // Up arrow was pressed
    if (data->EventKey == ImGuiKey_UpArrow) {
      if (!console->m_command_history.empty()) {
        if (console->m_history_pos == -1) {
          // If not browsing history, start from the latest command
          console->m_history_pos = console->m_command_history.size() - 1;
        } else if (console->m_history_pos > 0) {
          // Go to the previous command
          console->m_history_pos--;
        }
      }
    }
    // Down arrow was pressed
    else if (data->EventKey == ImGuiKey_DownArrow) {
      if (console->m_history_pos != -1) {
        if (console->m_history_pos <
            static_cast<int>(console->m_command_history.size()) - 1) {
          // Go to the next command
          console->m_history_pos++;
        } else {
          // Reached the end, clear the buffer and exit history mode
          console->m_history_pos = -1;
          data->DeleteChars(0, data->BufTextLen);
          return 0;
        }
      }
    }

    // If we are browsing history, update the input buffer
    if (console->m_history_pos != -1) {
      const std::string &history_command =
          console->m_command_history[console->m_history_pos];
      // Update the buffer in the ImGui widget
      data->DeleteChars(0, data->BufTextLen);
      data->InsertChars(0, history_command.c_str());
    }
  }
  return 0;
}

DebugConsole::DebugConsole()
    : m_input_buffer{0}, m_history_pos(-1), m_is_visible(false) {}

DebugConsole::~DebugConsole() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void DebugConsole::init(GLFWwindow *window) {
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  add_log("Welcome to the interactive console! Press ` to toggle.");
}

void DebugConsole::set_command_callback(
    const std::function<void(const std::string &)> &callback) {
  m_command_callback = callback;
}

void DebugConsole::add_log(const std::string &log) {
  m_log_history.push_back(log);
}

void DebugConsole::execute_command(const std::string &command) {
  if (command.empty()) {
    return;
  }

  // Add to log and history
  add_log("# " + command);
  m_command_history.push_back(command);
  m_history_pos = -1;

  // Execute the command via callback
  if (m_command_callback) {
    m_command_callback(command);
  }

  // Clear input buffer
  memset(m_input_buffer, 0, sizeof(m_input_buffer));
}

void DebugConsole::on_key_pressed(int key) {
  if (key == GLFW_KEY_GRAVE_ACCENT) { // The ` key
    m_is_visible = !m_is_visible;
  }
}

void DebugConsole::draw() {
  if (!m_is_visible) {
    return;
  }

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
  if (ImGui::Begin("Console", &m_is_visible)) {
    // Log history
    ImGui::BeginChild("ScrollingRegion",
                      ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false,
                      ImGuiWindowFlags_HorizontalScrollbar);
    for (const auto &log : m_log_history) {
      ImGui::TextUnformatted(log.c_str());
    }
    ImGui::SetScrollY(ImGui::GetScrollMaxY());
    ImGui::EndChild();

    ImGui::Separator();

    // Command input
    ImGuiInputTextFlags input_text_flags =
        ImGuiInputTextFlags_EnterReturnsTrue |
        ImGuiInputTextFlags_CallbackHistory;
    if (ImGui::InputText("Input", m_input_buffer, IM_ARRAYSIZE(m_input_buffer),
                         input_text_flags, &DebugConsole::text_edit_callback,
                         (void *)this)) {
      execute_command(m_input_buffer);
      ImGui::SetKeyboardFocusHere(-1); // Auto re-focus after entering
    }
  }
  ImGui::End();

  // Rendering
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
