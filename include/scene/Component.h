#pragma once

class SceneObject; // Forward-declaration

class Component {
public:
  virtual ~Component() = default;
  // Called when the component is attached to an object
  virtual void awake() {}
  // Called every frame
  virtual void update(float delta_time) {}

protected:
  SceneObject *m_owner = nullptr; // Pointer back to the owner object
  friend class SceneObject;
};
