#pragma once

#include "graphics/Mesh.h"
#include "scene/Component.h"
#include "scene/TransformComponent.h"
#include <memory>
#include <typeindex>
#include <unordered_map>

struct SceneObject {
  // Use a shared_ptr so multiple objects can share the same mesh data (e.g., a
  // forest of identical trees)
  std::shared_ptr<Mesh> mesh;
  std::shared_ptr<TransformComponent> transform;

  std::unordered_map<std::type_index, std::shared_ptr<Component>> m_components;

  SceneObject(std::shared_ptr<Mesh> m);
  SceneObject();

  void update(float delta_time);

  template <typename T, typename... Args>
  std::shared_ptr<T> add_component(Args &&...args) {
    auto new_comp = std::make_shared<T>(std::forward<Args>(args)...);
    new_comp->m_owner = this; // Set the owner
    new_comp->awake();
    m_components[std::type_index(typeid(T))] = new_comp;
    return new_comp;
  }

  // Finds and returns the first component of the specified type.
  template <typename T> std::shared_ptr<T> get_component() {
    auto it = m_components.find(std::type_index(typeid(T)));
    if (it != m_components.end()) {
      return std::static_pointer_cast<T>(it->second);
    }
    return nullptr;
  }
};
