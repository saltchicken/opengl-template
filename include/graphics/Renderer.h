#pragma once

#include <memory>

class Shader;
class Scene;
class Mesh;

class Renderer {
public:
  Renderer();
  ~Renderer();

  bool init(bool transparent_background);
  void update(float delta_time);
  void draw(Scene &scene, unsigned int screen_width,
            unsigned int screen_height);

private:
  std::shared_ptr<Shader> m_shader;
  std::shared_ptr<Shader> m_instanced_shader;
  std::shared_ptr<Shader> m_background_shader;
  std::shared_ptr<Mesh> m_background_quad_mesh;
  bool m_transparent_background;
};
