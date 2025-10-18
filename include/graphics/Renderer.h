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
  std::shared_ptr<Shader> m_shader;
  std::shared_ptr<Shader> m_instanced_shader; // <-- ADD THIS
  std::shared_ptr<Shader> m_background_shader;
  std::shared_ptr<Mesh> m_background_quad_mesh;
};
