#pragma once

#include <string>

// Forward declarations
class Scene;
struct Config;

class IRenderer {
public:
  virtual ~IRenderer() = default;

  // Initializes the renderer.
  virtual bool init(const Config &config) = 0;

  // Called once per frame for any state updates.
  virtual void update(float delta_time) = 0;

  // The main drawing function.
  virtual void draw(Scene &scene, unsigned int screen_width,
                    unsigned int screen_height) = 0;

  // A helper for Lua scripting to change shaders, etc.
  // We can provide a default empty implementation.
  virtual void execute_command(const std::string &command_line) {}
};
