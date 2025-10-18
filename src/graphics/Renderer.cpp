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

  if (m_transparent_background) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Transparent black
  } else {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Opaque dark grey
  }
  // 1. Create the shader program using our Shader class
  m_shader = ResourceManager::load_shader("default", "shaders/shader.vert",
                                          "shaders/shader.frag");
  if (!m_shader) {
    std::cerr << "Failed to load default shader." << std::endl;
    return false;
  }

  // --- NEW: Load the instanced shader ---
  m_instanced_shader = ResourceManager::load_shader(
      "instanced", "shaders/shader_instanced.vert", "shaders/shader.frag");
  if (!m_instanced_shader) {
    std::cerr << "Failed to load instanced shader." << std::endl;
    return false;
  }
  // --- END NEW ---

  if (!m_transparent_background) {
    m_background_shader = ResourceManager::load_shader(
        "background", "shaders/background.vert", "shaders/background.frag");
    if (!m_background_shader) {
      std::cerr << "Failed to load 'background' shader." << std::endl;
      return false;
    }
    m_background_quad_mesh = ResourceManager::get_primitive("quad");
  }

  std::cout << "Renderer initialized successfully." << std::endl;
  return true;
}

void Renderer::update(float delta_time) {
  // std::cout << delta_time << std::endl;
}

void Renderer::draw(Scene &scene, unsigned int screen_width,
                    unsigned int screen_height) {
  // --- Background drawing is unchanged ---
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (!m_transparent_background) {
    glDepthMask(GL_FALSE);
    m_background_shader->use();
    m_background_quad_mesh->draw(*m_background_shader);
    // Re-enable depth writing for the main scene.
    glDepthMask(GL_TRUE);
  }

  // --- Camera setup is unchanged ---
  auto camera_object = scene.get_active_camera();
  if (!camera_object) { /* ... error handling ... */
    return;
  }
  auto camera_comp = camera_object->get_component<CameraComponent>();
  if (!camera_comp) { /* ... error handling ... */
    return;
  }

  // --- Create matrices (once per frame) ---
  glm::mat4 view = camera_comp->get_view_matrix();
  float aspect_ratio =
      static_cast<float>(screen_width) / static_cast<float>(screen_height);
  glm::mat4 projection = camera_comp->get_projection_matrix(aspect_ratio);

  // --- UPDATE: Render loop with shader switching ---
  for (const auto &object : scene.get_scene_objects()) {
    if (object->mesh) {
      if (object->mesh->is_instanced()) {
        // Use the instanced shader
        m_instanced_shader->use();
        m_instanced_shader->set_mat4("projection", projection);
        m_instanced_shader->set_mat4("view", view);
        m_instanced_shader->set_mat4("model",
                                     object->transform->get_transform_matrix());
        object->mesh->draw(*m_instanced_shader);
      } else {
        // Use the regular shader for single objects
        m_shader->use();
        m_shader->set_mat4("projection", projection);
        m_shader->set_mat4("view", view);
        m_shader->set_mat4("model", object->transform->get_transform_matrix());
        object->mesh->draw(*m_shader);
      }
    }
  }
}
