#pragma once

#include "scene/Component.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class Shader; // Forward declaration

class IfsFractalComponent : public Component {
public:
  // num_points: how many points to generate
  // iterations: how many times to apply transforms for each point (higher =
  // more detail)
  IfsFractalComponent(size_t num_points = 100000, int iterations = 100);
  ~IfsFractalComponent();

  void awake() override;
  void update(float delta_time) override;

private:
  // A struct to hold one of our fractal's transformation rules
  struct AffineTransform {
    glm::mat4 matrix;
    // We can add color or other properties here later
  };

  size_t m_num_points;
  int m_iterations;

  std::shared_ptr<Shader> m_compute_shader;
  std::vector<AffineTransform> m_transforms;

  // SSBO for the fractal rules, SSBO for the resulting points
  unsigned int m_transforms_ssbo = 0;
  unsigned int m_points_ssbo = 0;
};
