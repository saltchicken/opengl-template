#include "scene/RotationComponent.h"
#include "scene/SceneObject.h" // To access the owner's transform
#include <glm/gtc/matrix_transform.hpp>

RotationComponent::RotationComponent(const glm::vec3 &axis,
                                     float degrees_per_second)
    : m_axis(axis), m_degrees_per_second(degrees_per_second) {}

void RotationComponent::update(float delta_time) {
  // Ensure we have a valid owner before trying to access its members
  if (m_owner) {
    // Apply rotation to the owner's transform matrix
    m_owner->transform =
        glm::rotate(m_owner->transform,
                    glm::radians(m_degrees_per_second) * delta_time, m_axis);
  }
}
