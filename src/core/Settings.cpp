#include "core/Settings.h"
#include "utils/Log.h"
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
    m_config.window_transparent =
        tbl["window"]["transparent"].value_or(m_config.window_transparent);
    m_config.fps = tbl["performance"]["fps"].value_or(m_config.fps);

    Log::info("Settings loaded successfully from " + filepath);
    return true;
  } catch (const toml::parse_error &err) {
    Log::error("Failed to parse " + filepath + ": " + err.what());
    Log::warn("Using default settings.");
    return false;
  } catch (const std::exception &e) {
    Log::error("An unexpected error occurred while reading " + filepath + ": " +
               e.what());
    Log::warn("Using default settings.");
    return false;
  }
}

const Config &Settings::get_config() const { return m_config; }
