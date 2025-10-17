#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float FOV = 90.0f;

// A simplified camera class for a static view.
class Camera {
public:
  // Camera Attributes
  glm::vec3 Position;
  float Fov;

  // Constructor with a position
  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));

  // Returns the view matrix
  glm::mat4 GetViewMatrix() const;
};
