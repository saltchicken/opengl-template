#include "scene/RotationComponent.h"
#include "scene/SceneObject.h" // To access the owner's transform
#include <glm/gtc/matrix_transform.hpp>

RotationComponent::RotationComponent(const glm::vec3 &axis,
                                     float degrees_per_second)
    : m_axis(axis), m_degrees_per_second(degrees_per_second) {}

void RotationComponent::update(float delta_time) {
  if (m_owner && m_owner->transform) {
    float radians = glm::radians(m_degrees_per_second) * delta_time;
    // Create a quaternion representing this frame's rotation
    glm::quat rotation_delta = glm::angleAxis(radians, m_axis);
    // Combine it with the existing rotation
    m_owner->transform->rotation =
        rotation_delta * m_owner->transform->rotation;
  }
}
