#include "graphics/Renderer.h"
#include "scene/Scene.h"
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

  Camera &camera = scene.get_camera();

  // Use the shader and draw the triangle
  m_shader->use();

  glm::mat4 view = camera.GetViewMatrix();
  glm::mat4 projection =
      glm::perspective(glm::radians(camera.Fov),
                       (float)screen_width / screen_height, 0.1f, 100.0f);

  m_shader->set_mat4("projection", projection);
  m_shader->set_mat4("view", view);

  for (const auto &object : scene.get_scene_objects()) {
    m_shader->set_mat4("model", object->transform->get_transform_matrix());
    object->mesh->draw(*m_shader);
  }
}
