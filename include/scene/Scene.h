#pragma once

#include "scene/Camera.h"
#include "scene/SceneObject.h"
#include <memory>
#include <vector>

class Scene {
public:
  // Constructor: Initializes the scene, including the default camera.
  Scene();

  // Adds a new object to the scene.
  void add_object(std::shared_ptr<SceneObject> object);

  // Provides const access to the list of objects for rendering.
  const std::vector<std::shared_ptr<SceneObject>> &get_scene_objects() const;

  // Provides access to the scene's camera.
  Camera &get_camera();

private:
  std::vector<std::shared_ptr<SceneObject>> m_scene_objects;
  Camera m_camera;
};
