#pragma once

#include <memory>

class Shader;
class Scene;
class Mesh;
class Config;

class Renderer {
public:
  Renderer();
  ~Renderer();

  bool init(const Config &config, unsigned int width, unsigned int height);
  void draw(Scene &scene, const Config &config, unsigned int screen_width,
            unsigned int screen_height);

  void on_window_resize(unsigned int width, unsigned int height);

private:
  void create_framebuffers();

  // Shaders
  std::shared_ptr<Shader> m_instanced_shader;
  std::shared_ptr<Shader> m_point_shader;
  std::shared_ptr<Shader> m_background_shader;
  std::shared_ptr<Shader> m_post_process_shader;

  // Primitives
  std::shared_ptr<Mesh> m_background_quad_mesh;
  std::shared_ptr<Mesh> m_screen_quad_mesh;

  // Framebuffers and Textures for multi-pass rendering
  unsigned int m_points_fbo = 0;
  unsigned int m_points_texture = 0;
  unsigned int m_accumulation_fbo[2] = {0, 0};
  unsigned int m_accumulation_texture[2] = {0, 0};
  int m_current_accumulation_index = 0;

  // Internal state
  bool m_transparent_background;
  unsigned int m_width = 0;
  unsigned int m_height = 0;
};
