#pragma once
#include "SceneObject.h"
#include <memory>
#include <vector>

class Shader; // Forward declaration
class Camera;

class Renderer {
public:
  Renderer();
  ~Renderer();

  bool init();
  void update(float delta_time);
  void submit(const std::shared_ptr<SceneObject> object);
  void draw(Camera &camera, unsigned int screen_width,
            unsigned int screen_height);

private:
  std::unique_ptr<Shader> m_shader; // Manages the shader's lifetime
  std::vector<std::shared_ptr<SceneObject>> m_scene_objects;
};
