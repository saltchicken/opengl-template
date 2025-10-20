#pragma once
#include "graphics/renderers/IRenderer.h"
#include <memory>

// Forward declarations
// ‼️ PointCloud is no longer used here
class Shader;
class SceneObject;

class PointCloudRenderer : public IRenderer {
public:
  PointCloudRenderer();
  ~PointCloudRenderer();

  bool init(const Config &config) override;
  void update(float delta_time) override;
  void draw(Scene &scene, unsigned int screen_width,
            unsigned int screen_height) override;

private:
  // ‼️ Replace PointCloud with raw OpenGL handles
  unsigned int m_vao = 0;
  unsigned int m_vbo_start = 0;
  unsigned int m_vbo_end = 0;
  int m_point_count = 0;

  std::shared_ptr<Shader> m_shader;

  std::shared_ptr<SceneObject> m_fractal_object;
  std::shared_ptr<SceneObject> m_camera_object;

  // ‼️ Add a member for the interpolation factor
  float m_interpolation_factor = 0.0f;
};
