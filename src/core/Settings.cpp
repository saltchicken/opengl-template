#include "core/Settings.h"
#include <iostream>
#include <toml++/toml.h>

Settings::Settings() = default;

bool Settings::load(const std::string &filepath) {
  try {
    toml::table tbl = toml::parse_file(filepath);

    // Safely extract values with fallbacks to defaults
    m_config.window_width =
        tbl["window"]["width"].value_or(m_config.window_width);
    m_config.window_height =
        tbl["window"]["height"].value_or(m_config.window_height);
    m_config.window_title =
        tbl["window"]["title"].value_or(m_config.window_title);
    m_config.window_resizable =
        tbl["window"]["resizable"].value_or(m_config.window_resizable);
    m_config.fps = tbl["performance"]["fps"].value_or(m_config.fps);

    std::cout << "Settings loaded successfully from " << filepath << std::endl;
    return true;
  } catch (const toml::parse_error &err) {
    std::cerr << "Failed to parse " << filepath << ":\n" << err << std::endl;
    std::cerr << "Using default settings." << std::endl;
    return false;
  } catch (const std::exception &e) {
    std::cerr << "An unexpected error occurred while reading " << filepath
              << ": " << e.what() << std::endl;
    std::cerr << "Using default settings." << std::endl;
    return false;
  }
}

const Config &Settings::get_config() const { return m_config; }
