#pragma once

#include "graphics/Mesh.h"
#include "scene/Component.h"
#include "scene/TransformComponent.h"
#include <memory>
#include <typeindex>
#include <unordered_map>

struct SceneObject : std::enable_shared_from_this<SceneObject> {
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
    // ‼️ This is not valid. You can't assign a raw `this` to a weak_ptr.
    // new_comp->m_owner = this;
    // Instead, we will need to pass a shared_ptr of this object later.
    // We can do this in the scene when the object is created and is a
    // shared_ptr Or, if SceneObject itself will be managed by a shared_ptr, it
    // should inherit from std::enable_shared_from_this Let's assume SceneObject
    // should manage this itself.

    // TODO: This is a hack. Fix this
    // This requires a bit more of a refactor. A common way is to make
    // add_component be called only on a shared_ptr<SceneObject> or have the
    // scene do it. A simpler immediate fix is to do it after creation. But the
    // BEST way is to use enable_shared_from_this.
    new_comp->m_owner = shared_from_this();
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
