#pragma once

#include "scene/Component.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class Shader; // Forward declaration

class IfsFractalComponent : public Component {
public:
  IfsFractalComponent(size_t num_points = 200000, int iterations = 150,
                      float denoise_factor = 0.5f);
  ~IfsFractalComponent();

  void awake() override;
  void update(float delta_time) override;

  float denoise_factor;

private:
  // A struct to hold one of our fractal's transformation rules
  struct AffineTransform {
    glm::mat4 matrix;
    // Pad to align with std140/std430 memory layout if more members are added
  };

  size_t m_num_points;
  int m_iterations;
  int m_num_base_transforms = 0; // The number of transforms *per fractal*

  std::shared_ptr<Shader> m_compute_shader;

  // SSBO for the fractal rules, SSBO for the resulting points
  unsigned int m_transforms_ssbo = 0;
  unsigned int m_points_ssbo = 0;
};
