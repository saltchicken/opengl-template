#pragma once

#include <string>

// A simple struct to hold our application's configuration.
struct Config {
  unsigned int window_width = 800;
  unsigned int window_height = 600;
  bool window_resizable = true;
  float fps = 60.0f;
  std::string window_title = "OpenGL Application";
};

// Manages loading and parsing the settings.toml file.
class Settings {
public:
  Settings();
  // Loads settings from the specified file path.
  bool load(const std::string &filepath);
  // Provides access to the loaded configuration.
  const Config &get_config() const;

private:
  Config m_config;
};
