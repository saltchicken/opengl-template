#include "scene/TransformComponent.h"
#include <glm/gtc/matrix_transform.hpp>

TransformComponent::TransformComponent(const glm::vec3 &pos,
                                       const glm::quat &rot,
                                       const glm::vec3 &scl)
    : position(pos), rotation(rot), scale(scl) {}

glm::mat4 TransformComponent::get_transform_matrix() const {
  // TRS: Translation * Rotation * Scale
  glm::mat4 trans_mat = glm::translate(glm::mat4(1.0f), position);
  glm::mat4 rot_mat = glm::mat4_cast(rotation);
  glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), scale);
  return trans_mat * rot_mat * scale_mat;
}
