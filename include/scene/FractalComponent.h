#pragma once

#include "scene/Component.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class Shader; // Forward declaration

class FractalComponent : public Component {
public:
  FractalComponent(int recursion_depth = 3,
                   float sphere_scale_multiplier = 1.0f,
                   float vibration_amplitude = 0.05f,
                   float vibration_speed = 3.0f);
  ~FractalComponent();

  void awake() override;
  void update(float delta_time) override;

private:
  void generate_sponge(const glm::vec3 &position, float scale, int depth);

  int m_recursion_depth;
  float m_sphere_scale_multiplier;
  float m_vibration_amplitude;
  float m_vibration_speed;

  std::shared_ptr<Shader> m_compute_shader;
  std::vector<glm::mat4> m_base_matrices;

  // OpenGL handles for the Shader Storage Buffer Objects (SSBOs)
  unsigned int m_base_matrices_ssbo = 0;
  unsigned int m_dynamic_matrices_ssbo = 0;
};
