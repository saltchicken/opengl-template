#include "Camera.h"

Camera::Camera(glm::vec3 position) : Position(position), Fov(FOV) {}

glm::mat4 Camera::GetViewMatrix() const {
  // Always look at the origin (0,0,0) from the camera's position.
  return glm::lookAt(Position, glm::vec3(0.0f, 0.0f, 0.0f),
                     glm::vec3(0.0f, 1.0f, 0.0f));
}
