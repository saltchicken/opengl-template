#pragma once
#include "graphics/renderers/IRenderer.h"
#include <memory>

// Forward declarations
class PointCloud;
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
  std::unique_ptr<PointCloud> m_point_cloud;
  std::shared_ptr<Shader> m_shader;

  // We will manage our own simple scene internally
  std::shared_ptr<SceneObject> m_fractal_object;
  std::shared_ptr<SceneObject> m_camera_object;
};
