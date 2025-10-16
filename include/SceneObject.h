#pragma once

#include "Mesh.h"
#include <memory>

struct SceneObject {
  // Use a shared_ptr so multiple objects can share the same mesh data (e.g., a
  // forest of identical trees)
  std::shared_ptr<Mesh> mesh;

  // Represents the object's position, rotation, and scale in the world
  glm::mat4 transform;

  SceneObject(std::shared_ptr<Mesh> m) : mesh(m), transform(glm::mat4(1.0f)) {}
};
