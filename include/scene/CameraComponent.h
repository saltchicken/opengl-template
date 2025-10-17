#pragma once
#include "scene/Component.h"
#include <glm/glm.hpp>

class CameraComponent : public Component {
public:
  float fov;
  float near_plane;
  float far_plane;

  CameraComponent(float fov = 45.0f, float near = 0.1f, float far = 100.0f);

  glm::mat4 get_view_matrix() const;
  glm::mat4 get_projection_matrix(float aspect_ratio) const;
};
