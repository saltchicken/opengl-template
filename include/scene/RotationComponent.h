#pragma once
#include "scene/Component.h"
#include <glm/glm.hpp>

class RotationComponent : public Component {
public:
  // Set rotation speed and axis in the constructor
  RotationComponent(const glm::vec3 &axis, float degrees_per_second);
  void update(float delta_time) override;

private:
  glm::vec3 m_axis;
  float m_degrees_per_second;
};
