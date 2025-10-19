#include "graphics/SceneRenderer.h"
#include "core/Settings.h"
#include "scene/CameraComponent.h"
#include "scene/Scene.h"
#include "utils/Log.h"
#include "utils/ResourceManager.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

SceneRenderer::SceneRenderer() = default;
SceneRenderer::~SceneRenderer() = default;

bool SceneRenderer::init(const Config &config) {
  m_transparent_background = config.window_transparent;
  glEnable(GL_DEPTH_TEST);

  if (m_transparent_background) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Transparent black
  } else {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Opaque dark grey
  }

  m_shader = ResourceManager::get_shader(config.main_shader_name);

  if (!m_shader) {
    Log::error("Failed to get shader '" + config.main_shader_name);
    return false;
  }

  if (!m_transparent_background) {
    m_background_shader =
        ResourceManager::get_shader(config.background_shader_name);
    if (!m_background_shader) {
      Log::error("Failed to get shader '" + config.background_shader_name);
      return false;
    }
    if (m_background_shader) {
      m_background_quad_mesh = ResourceManager::get_primitive("quad");
    }
  }

  Log::info("Renderer initialized successfully.");
  return true;
}

void SceneRenderer::update(float delta_time) {
  // std::cout << delta_time << std::endl;
}

void SceneRenderer::draw(Scene &scene, unsigned int screen_width,
                         unsigned int screen_height) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (!m_transparent_background && m_background_shader) {
    glDepthMask(GL_FALSE);
    m_background_shader->use();
    m_background_quad_mesh->draw(*m_background_shader);
    // Re-enable depth writing for the main scene.
    glDepthMask(GL_TRUE);
  }

  auto camera_object = scene.get_active_camera();
  // TODO: Is there a way to skip having to check if camera_object exists? Does
  // this provide any performance boost?
  if (!camera_object) {
    Log::error("No active camera object in the scene.");
    return; // No camera, nothing to render
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

void SceneRenderer::set_background_shader(const std::string &name) {
  auto new_shader = ResourceManager::get_shader(name);
  if (new_shader) {
    m_background_shader = new_shader;
    Log::info("Background shader switched to '" + name + "'");
  } else {
    Log::warn("Failed to find shader '" + name + "' for background.");
  }
}
