#include "scene/Scene.h"
#include "scene/CameraComponent.h"
#include "utils/Log.h"

Scene::Scene() {}

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

void Scene::set_active_camera(std::shared_ptr<SceneObject> camera_object) {
  if (camera_object && camera_object->get_component<CameraComponent>()) {
    m_active_camera = camera_object;
  } else {
    Log::warn("Attempted to set an active camera on an object that doesn't "
              "have a CameraComponent.");
  }
}

std::shared_ptr<SceneObject> Scene::get_active_camera() const {
  return m_active_camera.lock(); // .lock() converts weak_ptr to shared_ptr
}
