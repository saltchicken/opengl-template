#pragma once
#include "scene/Component.h"
#include <glm/glm.hpp>
#include <variant>

class PropertyAnimatorComponent : public Component {
public:
  // This enum is now the primary way to specify the animation type
  enum class TargetProperty { ROTATION, POSITION, SCALE };

  // Parameter structs remain the same
  struct RotationParams {
    glm::vec3 axis;
    float degrees_per_second;
  };
  struct PositionParams {
    glm::vec3 direction;
    float speed;
    float distance;
  };
  struct ScaleParams {
    float speed;
    float min_scale;
    float max_scale;
  };

  // Alias for our variant to make the constructor cleaner
  using AnimationParams =
      std::variant<RotationParams, PositionParams, ScaleParams>;

  // A single, explicit public constructor
  PropertyAnimatorComponent(TargetProperty target, AnimationParams params);

  void update(float delta_time) override;
  void awake() override;

private:
  TargetProperty m_target;
  AnimationParams m_params;

  glm::vec3 m_original_position;
  glm::vec3 m_original_scale;
};
