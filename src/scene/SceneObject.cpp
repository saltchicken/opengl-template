#include "scene/SceneObject.h"

// The constructor now just takes the mesh
SceneObject::SceneObject(std::shared_ptr<Mesh> m)
    : mesh(m), transform(std::make_shared<TransformComponent>()) {}

// <-- Add this new constructor implementation
SceneObject::SceneObject()
    : mesh(nullptr), transform(std::make_shared<TransformComponent>()) {}

// The update function loops through all components and calls their update
// methods
void SceneObject::update(float delta_time) {
  for (const auto &pair : m_components) {
    pair.second->update(delta_time);
  }
}
