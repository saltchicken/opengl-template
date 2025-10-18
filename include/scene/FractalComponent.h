#pragma once
#include "scene/Component.h"
#include <glm/glm.hpp>
#include <vector>

class FractalComponent : public Component {
public:
  // ADD sphere_scale_multiplier parameter with a default value of 1.0
  FractalComponent(int recursion_depth = 3,
                   float sphere_scale_multiplier = 1.0f);
  void awake() override;

private:
  void generate_sponge(const glm::vec3 &position, float scale, int depth);

  std::vector<glm::mat4> m_matrices;
  int m_recursion_depth;
  float m_sphere_scale_multiplier; // <-- ADD this member variable
};
