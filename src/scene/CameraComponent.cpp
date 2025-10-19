#include "scene/CameraComponent.h"
#include "scene/SceneObject.h"
#include <glm/gtc/matrix_transform.hpp>

CameraComponent::CameraComponent(float fov, float near_plane, float far_plane)
    : fov(fov), near_plane(near_plane), far_plane(far_plane) {}

glm::mat4 CameraComponent::get_view_matrix() const {
  // ‼️ Lock the weak_ptr to get a temporary shared_ptr
  if (auto owner = m_owner.lock()) {
    if (owner->transform) {
      return glm::inverse(owner->transform->get_transform_matrix());
    }
  }
  return glm::mat4(1.0f);
}

glm::mat4 CameraComponent::get_projection_matrix(float aspect_ratio) const {
  return glm::perspective(glm::radians(fov), aspect_ratio, near_plane,
                          far_plane);
}
