#pragma once
#include <memory>
#include <string>

class Shader;
class Scene;
class Mesh;
struct Config;

class Renderer {
public:
  Renderer();
  ~Renderer();

  bool init(const Config &config);
  void update(float delta_time);
  void draw(Scene &scene, unsigned int screen_width,
            unsigned int screen_height);
  void set_background_shader(const std::string &name);

private:
  std::shared_ptr<Shader> m_shader; // Manages the shader's lifetime
  std::shared_ptr<Shader> m_background_shader;
  std::shared_ptr<Mesh> m_background_quad_mesh;

  bool m_transparent_background;
};
