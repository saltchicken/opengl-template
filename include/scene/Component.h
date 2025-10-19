#pragma once

#include <memory>

class SceneObject; // Forward-declaration

class Component {
public:
  virtual ~Component() = default;
  // Called when the component is attached to an object
  virtual void awake() {}
  // Called every frame
  virtual void update(float delta_time) {}

protected:
  std::weak_ptr<SceneObject> m_owner;
  friend class SceneObject;
};
