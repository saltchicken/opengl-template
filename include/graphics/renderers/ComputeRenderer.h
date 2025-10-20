#pragma once

#include "graphics/renderers/IRenderer.h"
#include <memory>

class Shader;
class Mesh;

class ComputeRenderer : public IRenderer {
public:
  ComputeRenderer();
  ~ComputeRenderer();

  bool init(const Config &config) override;
  void update(float delta_time) override;
  void draw(Scene &scene, unsigned int screen_width,
            unsigned int screen_height) override;

private:
  void create_texture(unsigned int width, unsigned int height);

  std::shared_ptr<Shader> m_compute_shader;
  std::shared_ptr<Shader> m_draw_shader;
  std::shared_ptr<Mesh> m_quad_mesh;

  unsigned int m_texture_id = 0;
  unsigned int m_texture_width = 0;
  unsigned int m_texture_height = 0;
};
