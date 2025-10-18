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

bool Renderer::init(bool transparent_background) {
  m_transparent_background = transparent_background;
  glEnable(GL_DEPTH_TEST);
  // --- NEW: Enable point size control in vertex shader ---
  glEnable(GL_PROGRAM_POINT_SIZE);

  if (m_transparent_background) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  } else {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  }

  m_shader = ResourceManager::load_shader("default", "shaders/shader.vert",
                                          "shaders/shader.frag");
  if (!m_shader)
    return false;

  m_instanced_shader = ResourceManager::load_shader(
      "instanced", "shaders/shader_instanced.vert", "shaders/shader.frag");
  if (!m_instanced_shader)
    return false;

  // --- NEW: Load point rendering shaders ---
  m_point_shader = ResourceManager::load_shader("points", "shaders/points.vert",
                                                "shaders/points.frag");
  if (!m_point_shader)
    return false;

  if (!m_transparent_background) {
    m_background_shader = ResourceManager::load_shader(
        "background", "shaders/background.vert", "shaders/background.frag");
    if (!m_background_shader)
      return false;
    m_background_quad_mesh = ResourceManager::get_primitive("quad");
  }

  std::cout << "Renderer initialized successfully." << std::endl;
  return true;
}

void Renderer::update(float delta_time) {}

void Renderer::draw(Scene &scene, unsigned int screen_width,
                    unsigned int screen_height) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (!m_transparent_background) {
    glDepthMask(GL_FALSE);
    m_background_shader->use();
    m_background_quad_mesh->draw(*m_background_shader);
    glDepthMask(GL_TRUE);
  }

  auto camera_object = scene.get_active_camera();
  if (!camera_object)
    return;
  auto camera_comp = camera_object->get_component<CameraComponent>();
  if (!camera_comp)
    return;

  glm::mat4 view = camera_comp->get_view_matrix();
  float aspect_ratio =
      static_cast<float>(screen_width) / static_cast<float>(screen_height);
  glm::mat4 projection = camera_comp->get_projection_matrix(aspect_ratio);

  // --- UPDATE: Render loop with logic for all three modes ---
  for (const auto &object : scene.get_scene_objects()) {
    if (object->mesh) {
      switch (object->mesh->render_mode) {
      case Mesh::RenderMode::POINTS:
        m_point_shader->use();
        m_point_shader->set_mat4("projection", projection);
        m_point_shader->set_mat4("view", view);
        m_point_shader->set_mat4("model",
                                 object->transform->get_transform_matrix());
        object->mesh->draw(*m_point_shader);
        break;

      case Mesh::RenderMode::TRIANGLES:
        if (object->mesh->is_instanced()) {
          m_instanced_shader->use();
          m_instanced_shader->set_mat4("projection", projection);
          m_instanced_shader->set_mat4("view", view);
          m_instanced_shader->set_mat4(
              "model", object->transform->get_transform_matrix());
          object->mesh->draw(*m_instanced_shader);
        } else {
          m_shader->use();
          m_shader->set_mat4("projection", projection);
          m_shader->set_mat4("view", view);
          m_shader->set_mat4("model",
                             object->transform->get_transform_matrix());
          object->mesh->draw(*m_shader);
        }
        break;
      }
    }
  }
}
