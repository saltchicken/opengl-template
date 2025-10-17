#pragma once
#include "scene/Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class TransformComponent : public Component {
public:
  glm::vec3 position;
  glm::quat rotation;
  glm::vec3 scale;

  TransformComponent(const glm::vec3 &pos = glm::vec3(0.0f),
                     const glm::quat &rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                     const glm::vec3 &scl = glm::vec3(1.0f));

  // Calculates and returns the model matrix
  glm::mat4 get_transform_matrix() const;
};
