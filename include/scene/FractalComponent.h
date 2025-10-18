#pragma once
#include "scene/Component.h"
#include <glm/glm.hpp>
#include <vector>

class FractalComponent : public Component {
public:
  FractalComponent(int recursion_depth = 3);
  void awake() override;

private:
  void generate_sponge(const glm::vec3 &position, float scale, int depth);

  std::vector<glm::mat4> m_matrices;
  int m_recursion_depth;
};
