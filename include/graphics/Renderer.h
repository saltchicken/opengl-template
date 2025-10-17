#pragma once
#include <memory>

class Shader; // Forward declaration
class Scene;

class Renderer {
public:
  Renderer();
  ~Renderer();

  bool init();
  void update(float delta_time);
  void draw(Scene &scene, unsigned int screen_width,
            unsigned int screen_height);

private:
  std::shared_ptr<Shader> m_shader; // Manages the shader's lifetime
};
