#include "graphics/Renderer.h"
#include "scene/CameraComponent.h"
#include "scene/Scene.h"
#include "utils/Log.h"
#include "utils/ResourceManager.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Renderer::Renderer() = default;
Renderer::~Renderer() = default;

bool Renderer::init() {
  glEnable(GL_DEPTH_TEST);
  // 1. Create the shader program using our Shader class
  m_shader = ResourceManager::load_shader("default", "shaders/shader.vert",
                                          "shaders/shader.frag");

  if (!m_shader) {
    std::cerr << "Failed to load default shader." << std::endl;
    return false;
  }

  m_background_shader = ResourceManager::load_shader(
      "background", "shaders/background.vert", "shaders/background.frag");
  if (!m_background_shader) {
    std::cerr << "Failed to load 'background' shader." << std::endl;
    return false;
  }

  m_background_quad_mesh = ResourceManager::get_primitive("quad");

  std::cout << "Renderer initialized successfully." << std::endl;
  return true;
}

void Renderer::update(float delta_time) {
  // std::cout << delta_time << std::endl;
}

void Renderer::draw(Scene &scene, unsigned int screen_width,
                    unsigned int screen_height) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDepthMask(GL_FALSE);
  m_background_shader->use();
  m_background_quad_mesh->draw(*m_background_shader);
  // Re-enable depth writing for the main scene.
  glDepthMask(GL_TRUE);

  auto camera_object = scene.get_active_camera();
  if (!camera_object) {
    Log::error("No active camera object in the scene.");
    return; // No camera, no rendering
  }

  // 2. Get the required components from the camera object
  auto camera_comp = camera_object->get_component<CameraComponent>();
  if (!camera_comp) {
    Log::error("Active camera object is missing its CameraComponent.");
    return;
  }

  // Use the shader and draw the triangle
  m_shader->use();

  glm::mat4 view = camera_comp->get_view_matrix();
  float aspect_ratio =
      static_cast<float>(screen_width) / static_cast<float>(screen_height);
  glm::mat4 projection = camera_comp->get_projection_matrix(aspect_ratio);

  m_shader->set_mat4("projection", projection);
  m_shader->set_mat4("view", view);

  for (const auto &object : scene.get_scene_objects()) {
    // Only draw objects that have a mesh component
    if (object->mesh) {
      m_shader->set_mat4("model", object->transform->get_transform_matrix());
      object->mesh->draw(*m_shader);
    }
  }
}
