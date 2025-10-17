#include "scene/CameraComponent.h"
#include "scene/SceneObject.h"
#include <glm/gtc/matrix_transform.hpp>

CameraComponent::CameraComponent(float fov, float near_plane, float far_plane)
    : fov(fov), near_plane(near_plane), far_plane(far_plane) {}

glm::mat4 CameraComponent::get_view_matrix() const {
  if (!m_owner || !m_owner->transform) {
    return glm::mat4(1.0f);
  }
  // The view matrix is the inverse of the camera's world transform.
  // This elegantly handles both position and rotation from the
  // TransformComponent.
  return glm::inverse(m_owner->transform->get_transform_matrix());
}

glm::mat4 CameraComponent::get_projection_matrix(float aspect_ratio) const {
  return glm::perspective(glm::radians(fov), aspect_ratio, near_plane,
                          far_plane);
}
