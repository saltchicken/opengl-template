#pragma once
#include "graphics/IRenderer.h"
#include <memory>
#include <string>

class Shader;
class Scene;
class Mesh;
struct Config;

class SceneRenderer : public IRenderer {
public:
  SceneRenderer();
  ~SceneRenderer();

  // Use the override keyword for clarity and safety
  bool init(const Config &config) override;
  void update(float delta_time) override;
  void draw(Scene &scene, unsigned int screen_width,
            unsigned int screen_height) override;

  void set_canvas_shader(const std::string &name);

private:
  std::shared_ptr<Shader> m_shader;
  std::shared_ptr<Shader> m_canvas_shader;
  std::shared_ptr<Mesh> m_canvas_quad_mesh;
  bool m_transparent_canvas;
};
