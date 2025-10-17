#pragma once

#include "scene/SceneObject.h"
#include <memory>
#include <vector>

class Scene {
public:
  // Constructor: Initializes the scene, including the default camera.
  Scene();

  // Adds a new object to the scene.
  void add_object(std::shared_ptr<SceneObject> object);

  void update(float delta_time);

  // Provides const access to the list of objects for rendering.
  const std::vector<std::shared_ptr<SceneObject>> &get_scene_objects() const;

  void set_active_camera(std::shared_ptr<SceneObject> camera_object);
  std::shared_ptr<SceneObject> get_active_camera() const;

private:
  std::vector<std::shared_ptr<SceneObject>> m_scene_objects;
  std::weak_ptr<SceneObject> m_active_camera;
};
