#include "scene/PropertyAnimatorComponent.h"
#include "core/Time.h"
#include "scene/SceneObject.h"
#include "utils/Log.h"
#include <cmath> // For sin()
#include <glm/gtc/quaternion.hpp>

PropertyAnimatorComponent::PropertyAnimatorComponent(TargetProperty target,
                                                     AnimationParams params)
    : m_target(target), m_params(std::move(params)) {}

void PropertyAnimatorComponent::awake() {
  // Lock the weak_ptr to get a temporary shared_ptr.
  if (auto owner = m_owner.lock()) {
    if (!owner->transform) {
      Log::error("PropertyAnimatorComponent: Owner has no transform");
      return;
    }
    // Use the locked shared_ptr to access members.
    if (m_target == TargetProperty::POSITION) {
      m_original_position = owner->transform->position;
    } else if (m_target == TargetProperty::SCALE) {
      m_original_scale = owner->transform->scale;
    }
  } else {
    Log::error("PropertyAnimatorComponent: Awake() called with no owner.");
  }
}

void PropertyAnimatorComponent::update(float delta_time) {
  // ‼️ Lock the owner once at the beginning of the update.
  auto owner = m_owner.lock();
  if (!owner || !owner->transform) {
    // The owner has been destroyed, so there's nothing to do.
    return;
  }

  double total_time = Time::get_total_time();
  // 'std::visit' safely unpacks the variant.
  std::visit(
      // The lambda captures the locked `owner` shared_ptr.
      [&](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, RotationParams>) {
          float radians = glm::radians(arg.degrees_per_second) * delta_time;
          glm::quat rotation_delta = glm::angleAxis(radians, arg.axis);
          owner->transform->rotation =
              rotation_delta * owner->transform->rotation;
        } else if constexpr (std::is_same_v<T, PositionParams>) {
          float offset = sin(total_time * arg.speed) * arg.distance;
          owner->transform->position =
              m_original_position + arg.direction * offset;
        } else if constexpr (std::is_same_v<T, ScaleParams>) {
          float range = arg.max_scale - arg.min_scale;
          float sine_wave_normalized =
              (sin(total_time * arg.speed) + 1.0f) / 2.0f;
          float current_scale_val =
              arg.min_scale + (sine_wave_normalized * range);
          owner->transform->scale = m_original_scale * current_scale_val;
        }
      },
      m_params); // Visiting our m_params member
}
