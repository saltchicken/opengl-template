#include "graphics/Renderer.h"
#include "core/Settings.h"
#include "scene/CameraComponent.h"
#include "scene/IfsFractalComponent.h"
#include "scene/Scene.h"
#include "utils/Log.h"
#include "utils/ResourceManager.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Renderer::Renderer() = default;

Renderer::~Renderer() {
  glDeleteFramebuffers(1, &m_points_fbo);
  glDeleteTextures(1, &m_points_texture);
  glDeleteFramebuffers(2, m_accumulation_fbo);
  glDeleteTextures(2, m_accumulation_texture);
}

bool Renderer::init(const Config &config, unsigned int width,
                    unsigned int height) {
  m_transparent_background = config.window_transparent;
  m_width = width;
  m_height = height;

  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_BLEND);

  if (m_transparent_background) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  } else {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  }

  m_instanced_shader = ResourceManager::load_shader(
      "instanced", "shaders/shader_instanced.vert", "shaders/shader.frag");
  m_point_shader = ResourceManager::load_shader("points", "shaders/points.vert",
                                                "shaders/points.frag");
  m_post_process_shader = ResourceManager::load_shader(
      "post", "shaders/post.vert", "shaders/post.frag");

  if (!m_instanced_shader || !m_point_shader || !m_post_process_shader)
    return false;

  if (!m_transparent_background) {
    m_background_shader = ResourceManager::load_shader(
        "background", "shaders/background.vert", "shaders/background.frag");
    if (!m_background_shader)
      return false;
    m_background_quad_mesh = ResourceManager::get_primitive("quad");
  }

  m_screen_quad_mesh = ResourceManager::get_primitive("quad");

  create_framebuffers();

  std::cout << "Renderer initialized successfully." << std::endl;
  return true;
}

void Renderer::on_window_resize(unsigned int width, unsigned int height) {
  if (width == 0 || height == 0)
    return;
  m_width = width;
  m_height = height;
  create_framebuffers();
  Log::info("Renderer resized, framebuffers recreated.");
}

void Renderer::create_framebuffers() {
  // Clean up old resources first
  glDeleteFramebuffers(1, &m_points_fbo);
  glDeleteTextures(1, &m_points_texture);
  glDeleteFramebuffers(2, m_accumulation_fbo);
  glDeleteTextures(2, m_accumulation_texture);

  // --- FBO for raw points ---
  glGenFramebuffers(1, &m_points_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_points_fbo);
  glGenTextures(1, &m_points_texture);
  glBindTexture(GL_TEXTURE_2D, m_points_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA,
               GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         m_points_texture, 0);

  // --- FBOs for accumulation (ping-pong) ---
  glGenFramebuffers(2, m_accumulation_fbo);
  glGenTextures(2, m_accumulation_texture);
  for (int i = 0; i < 2; ++i) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_accumulation_fbo[i]);
    glBindTexture(GL_TEXTURE_2D, m_accumulation_texture[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA,
                 GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           m_accumulation_texture[i], 0);
    // Clear the accumulation buffers initially
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    Log::error("Framebuffer is not complete!");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::draw(Scene &scene, const Config &config,
                    unsigned int screen_width, unsigned int screen_height) {
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

  // === PASS 1: Render Raw Points to FBO ===
  glBindFramebuffer(GL_FRAMEBUFFER, m_points_fbo);
  glViewport(0, 0, m_width, m_height);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending for points

  m_point_shader->use();
  m_point_shader->set_mat4("projection", projection);
  m_point_shader->set_mat4("view", view);
  m_point_shader->set_float("u_max_point_size", 50.0f);
  m_point_shader->set_float("u_point_alpha", config.fractal_point_alpha);

  for (const auto &object : scene.get_scene_objects()) {
    if (object->mesh && object->mesh->render_mode == Mesh::RenderMode::POINTS) {
      m_point_shader->set_mat4("model",
                               object->transform->get_transform_matrix());
      object->mesh->draw(*m_point_shader);
    }
  }

  // === PASS 2: Accumulation Pass ===
  int source_idx = m_current_accumulation_index;
  int dest_idx = (m_current_accumulation_index + 1) % 2;

  glBindFramebuffer(GL_FRAMEBUFFER, m_accumulation_fbo[dest_idx]);
  glViewport(0, 0, m_width, m_height);
  glDisable(GL_BLEND); // Blending is done manually in the shader

  m_post_process_shader->use();
  m_post_process_shader->set_bool("u_is_display_pass", false);
  m_post_process_shader->set_float("u_denoise_factor",
                                   config.fractal_denoise_factor);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_points_texture);
  m_post_process_shader->set_int("u_raw_points_texture", 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, m_accumulation_texture[source_idx]);
  m_post_process_shader->set_int("u_accumulation_texture", 1);

  m_screen_quad_mesh->draw(*m_post_process_shader);

  // === PASS 3: Final Display to Screen ===
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, screen_width, screen_height);
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  if (!m_transparent_background) {
    glDisable(GL_BLEND);
    m_background_shader->use();
    m_background_quad_mesh->draw(*m_background_shader);
  }

  glBlendFunc(GL_SRC_ALPHA,
              GL_ONE_MINUS_SRC_ALPHA); // Normal blending for UI/etc.
  m_post_process_shader->use();
  m_post_process_shader->set_bool("u_is_display_pass", true);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_accumulation_texture[dest_idx]);
  m_post_process_shader->set_int("u_accumulation_texture", 0);

  m_screen_quad_mesh->draw(*m_post_process_shader);

  // Ping-pong the buffers for the next frame
  m_current_accumulation_index = dest_idx;
}
