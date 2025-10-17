#pragma once
#include "scene/Component.h"
#include <glm/glm.hpp>

class PropertyAnimatorComponent : public Component {
public:
  // Defines which property of the TransformComponent we will animate
  enum class TargetProperty { ROTATION, POSITION, SCALE };

  // Parameters for rotation
  struct RotationParams {
    glm::vec3 axis;
    float degrees_per_second;
  };

  // Parameters for a simple sinusoidal position movement (bobbing)
  struct PositionParams {
    glm::vec3 direction;
    float speed;
    float distance;
  };

  // You could add more parameter structs for other animation types!

  // Constructor for Rotation
  PropertyAnimatorComponent(const glm::vec3 &axis, float degrees_per_second);

  // Constructor for Position
  PropertyAnimatorComponent(const glm::vec3 &direction, float speed,
                            float distance);

  void update(float delta_time) override;
  void awake() override;

private:
  TargetProperty m_target;

  // A union is a good way to store params since only one set is active at a
  // time
  union {
    RotationParams m_rot_params;
    PositionParams m_pos_params;
  };

  // We need to store the original position for sine wave movement
  glm::vec3 m_original_position;
};
