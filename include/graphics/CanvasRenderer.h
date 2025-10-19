#pragma once
#include "graphics/IRenderer.h"
#include <memory>

class Shader;
class Mesh;

class CanvasRenderer : public IRenderer {
public:
  CanvasRenderer();
  ~CanvasRenderer();

  bool init(const Config &config) override;
  void update(float delta_time) override;
  void draw(Scene &scene, unsigned int screen_width,
            unsigned int screen_height) override;

  void execute_command(const std::string &command_line) override;

private:
  std::shared_ptr<Shader> m_canvas_shader;
  std::shared_ptr<Mesh> m_canvas_quad_mesh;
};
