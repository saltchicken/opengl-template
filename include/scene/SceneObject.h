#pragma once

#include "graphics/Mesh.h"
#include "scene/Component.h"
#include "scene/TransformComponent.h"
#include <memory>
#include <vector>

struct SceneObject {
  // Use a shared_ptr so multiple objects can share the same mesh data (e.g., a
  // forest of identical trees)
  std::shared_ptr<Mesh> mesh;
  std::shared_ptr<TransformComponent> transform;

  std::vector<std::shared_ptr<Component>> m_components;

  SceneObject(std::shared_ptr<Mesh> m);

  void update(float delta_time);

  template <typename T, typename... Args>
  std::shared_ptr<T> add_component(Args &&...args) {
    auto new_comp = std::make_shared<T>(std::forward<Args>(args)...);
    new_comp->m_owner = this; // Set the owner
    new_comp->awake();
    m_components.push_back(new_comp);
    return new_comp;
  }
};
