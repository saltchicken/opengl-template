#include "scene/PropertyAnimatorComponent.h"
#include "scene/SceneObject.h"
#include <GLFW/glfw3.h> // For glfwGetTime()
#include <cmath>        // For sin()
#include <glm/gtc/quaternion.hpp>

// Constructor for Rotation
PropertyAnimatorComponent::PropertyAnimatorComponent(const glm::vec3 &axis,
                                                     float degrees_per_second)
    : m_target(TargetProperty::ROTATION) {
  m_rot_params.axis = axis;
  m_rot_params.degrees_per_second = degrees_per_second;
}

// Constructor for Position (a bobbing motion)
PropertyAnimatorComponent::PropertyAnimatorComponent(const glm::vec3 &direction,
                                                     float speed,
                                                     float distance)
    : m_target(TargetProperty::POSITION) {
  m_pos_params.direction = glm::normalize(direction);
  m_pos_params.speed = speed;
  m_pos_params.distance = distance;
}

void PropertyAnimatorComponent::awake() {
  // Store the owner's starting position if we are animating position
  if (m_owner && m_owner->transform && m_target == TargetProperty::POSITION) {
    m_original_position = m_owner->transform->position;
  }
}

void PropertyAnimatorComponent::update(float delta_time) {
  if (!m_owner || !m_owner->transform) {
    return;
  }

  switch (m_target) {
  case TargetProperty::ROTATION: {
    float radians = glm::radians(m_rot_params.degrees_per_second) * delta_time;
    glm::quat rotation_delta = glm::angleAxis(radians, m_rot_params.axis);
    m_owner->transform->rotation =
        rotation_delta * m_owner->transform->rotation;
    break;
  }

  case TargetProperty::POSITION: {
    // Calculate a bobbing motion using a sine wave
    float total_time = static_cast<float>(glfwGetTime());
    float offset = sin(total_time * m_pos_params.speed) * m_pos_params.distance;
    m_owner->transform->position =
        m_original_position + m_pos_params.direction * offset;
    break;
  }

  case TargetProperty::SCALE: {
    // Example: A pulsing scale effect could be implemented here
    break;
  }
  }
}
