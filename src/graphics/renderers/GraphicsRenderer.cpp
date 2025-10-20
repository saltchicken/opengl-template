#include "graphics/renderers/GraphicsRenderer.h"
#include "core/Settings.h"
#include "scene/CameraComponent.h"
#include "scene/Scene.h"
#include "utils/Log.h"
#include "utils/ResourceManager.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>

GraphicsRenderer::GraphicsRenderer() = default;
GraphicsRenderer::~GraphicsRenderer() = default;

bool GraphicsRenderer::init(const Config &config) {
  glEnable(GL_DEPTH_TEST);

  if (config.window_transparent) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Transparent black
  } else {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Opaque dark grey
  }

  m_shader = ResourceManager::get_shader(config.graphics_main_shader_name);

  if (!m_shader) {
    Log::error("Failed to get main shader '" +
               config.graphics_main_shader_name);
    return false;
  }

  m_canvas_shader =
      ResourceManager::get_shader(config.graphics_canvas_shader_name);
  m_canvas_quad_mesh = ResourceManager::get_primitive("quad");

  if (!m_canvas_shader || !m_canvas_quad_mesh) {
    Log::error("Failed to get canvas shader '" +
               config.graphics_canvas_shader_name);
    return false;
  }

  Log::info("Renderer initialized successfully.");
  return true;
}

void GraphicsRenderer::update(float delta_time) {
  // std::cout << delta_time << std::endl;
}

void GraphicsRenderer::draw(Scene &scene, unsigned int screen_width,
                            unsigned int screen_height) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDepthMask(GL_FALSE);
  m_canvas_shader->use();
  m_canvas_quad_mesh->draw(*m_canvas_shader);
  // Re-enable depth writing for the main scene.
  glDepthMask(GL_TRUE);

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

void GraphicsRenderer::execute_command(const std::string &command_line) {
  std::stringstream ss(command_line);
  std::string command;
  ss >> command; // Read the first word as the command

  if (command == "set_canvas_shader") {
    std::string shader_name;
    ss >> shader_name; // Read the next word as the argument
    if (!shader_name.empty()) {
      auto new_shader = ResourceManager::get_shader(shader_name);
      if (new_shader) {
        m_canvas_shader = new_shader;
        Log::info("Canvas shader switched to '" + shader_name + "'");
      } else {
        Log::warn("Failed to find shader '" + shader_name + "' for canvas.");
      }
    } else {
      Log::warn("Command 'set_canvas_shader' requires a shader name argument.");
    }
  }
  // Future extensibility example:
  // else if (command == "set_clear_color") {
  //   float r, g, b;
  //   ss >> r >> g >> b;
  //   glClearColor(r, g, b, 1.0f);
  //   Log::info("Clear color set.");
  // }
  else {
    Log::warn("GraphicsRenderer received unknown command: '" + command + "'");
  }
}
