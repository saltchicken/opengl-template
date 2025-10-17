#include "scene/PropertyAnimatorComponent.h"
#include "scene/SceneObject.h"
#include <GLFW/glfw3.h> // For glfwGetTime()
#include <cmath>        // For sin()
#include <glm/gtc/quaternion.hpp>

PropertyAnimatorComponent::PropertyAnimatorComponent(TargetProperty target,
                                                     AnimationParams params)
    : m_target(target), m_params(std::move(params)) {}

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

  float total_time = static_cast<float>(glfwGetTime());

  // 'std::visit' safely unpacks the variant. The 'arg' variable below
  // becomes a reference to whichever struct is currently active inside
  // m_params.
  std::visit(
      [&](auto &&arg) {
        // 'decay_t' gets the underlying type of 'arg' (e.g., RotationParams)
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, RotationParams>) {
          // Here, 'arg' IS the RotationParams struct
          float radians = glm::radians(arg.degrees_per_second) * delta_time;
          glm::quat rotation_delta = glm::angleAxis(radians, arg.axis);
          m_owner->transform->rotation =
              rotation_delta * m_owner->transform->rotation;
        } else if constexpr (std::is_same_v<T, PositionParams>) {
          // Here, 'arg' IS the PositionParams struct
          float offset = sin(total_time * arg.speed) * arg.distance;
          m_owner->transform->position =
              m_original_position + arg.direction * offset;
        } else if constexpr (std::is_same_v<T, ScaleParams>) {
          // Here, 'arg' IS the ScaleParams struct
          float range = arg.max_scale - arg.min_scale;
          float sine_wave_normalized =
              (sin(total_time * arg.speed) + 1.0f) / 2.0f;
          float current_scale_val =
              arg.min_scale + (sine_wave_normalized * range);
          m_owner->transform->scale = m_original_scale * current_scale_val;
        }
      },
      m_params); // We are visiting our m_params member
}
