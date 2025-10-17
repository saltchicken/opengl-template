#include "scene/Scene.h"

Scene::Scene()
    // Initialize the camera looking at the origin from 3 units away on the
    // Z-axis. The Scene now creates and owns the Camera directly.
    : m_camera(glm::vec3(0.0f, 0.0f, 3.0f)) {}

void Scene::add_object(std::shared_ptr<SceneObject> object) {
  m_scene_objects.push_back(object);
}

void Scene::update(float delta_time) {
  for (const auto &object : m_scene_objects) {
    object->update(delta_time);
  }
}

// We return a const reference to avoid making a copy of the entire vector
// every frame.
const std::vector<std::shared_ptr<SceneObject>> &
Scene::get_scene_objects() const {
  return m_scene_objects;
}

// We return a non-const reference so that we can modify the camera later
// (e.g., for movement).
Camera &Scene::get_camera() { return m_camera; }
