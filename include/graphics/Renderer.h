#pragma once
#include <memory>

class Shader; // Forward declaration
class Scene;
class Mesh;

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
  std::shared_ptr<Shader> m_background_shader;
  std::shared_ptr<Mesh> m_background_mesh;
};
