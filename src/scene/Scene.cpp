#include "scene/Scene.h"
#include "scene/CameraComponent.h"

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
  // It's good practice to ensure the object actually has a camera component
  if (camera_object) { // You could add a GetComponent<T> method for a cleaner
                       // check
    bool has_camera_comp = false;
    for (const auto &comp : camera_object->m_components) {
      if (std::dynamic_pointer_cast<CameraComponent>(comp)) {
        has_camera_comp = true;
        break;
      }
    }
    if (has_camera_comp) {
      m_active_camera = camera_object;
    }
  }
}

std::shared_ptr<SceneObject> Scene::get_active_camera() const {
  return m_active_camera.lock(); // .lock() converts weak_ptr to shared_ptr
}
