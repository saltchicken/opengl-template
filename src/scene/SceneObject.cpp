#include "scene/SceneObject.h"

// The constructor now just takes the mesh
SceneObject::SceneObject(std::shared_ptr<Mesh> m)
    : mesh(m), transform(glm::mat4(1.0f)) {}

// The update function loops through all components and calls their update
// methods
void SceneObject::update(float delta_time) {
  for (const auto &component : m_components) {
    component->update(delta_time);
  }
}
