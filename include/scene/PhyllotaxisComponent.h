#pragma once

#include "scene/Component.h"
#include <glm/glm.hpp>
#include <vector>

class PhyllotaxisComponent : public Component {
public:
  // Constructor: specify number of points and the overall radius
  PhyllotaxisComponent(int num_points = 2000, float radius = 3.0f);

  // Called when the component is added to the SceneObject
  void awake() override;

private:
  std::vector<glm::mat4> m_matrices;
  int m_num_points;
  float m_radius;
};
