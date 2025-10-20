#pragma once

#include <string>

// A simple struct to hold our application's configuration.
struct Config {
  // Initialization settings loaded from settings.toml
  unsigned int window_width = 800;
  unsigned int window_height = 600;
  bool window_resizable = true;
  bool window_transparent = false;
  float fps = 60.0f;
  std::string window_title = "OpenGL Application";

  // Runtime-configurable settings loaded from Lua
  std::string renderer_type = "graphics";

  // For GraphicsRenderer
  std::string graphics_main_shader_name = "default";
  std::string graphics_canvas_shader_name = "canvas";

  // For CanvasRenderer
  std::string canvas_shader_name = "canvas";

  // For ComputeRenderer
  std::string compute_shader_name = "compute_test";
  std::string compute_draw_shader_name = "draw_texture";
};

// Manages loading and parsing the settings.toml file.
class Settings {
public:
  Settings();
  // Loads settings from the specified file path.
  bool load(const std::string &filepath);
  // Provides access to the loaded configuration.
  const Config &get_config() const;
  Config &get_mutable_config();

private:
  Config m_config;
};
