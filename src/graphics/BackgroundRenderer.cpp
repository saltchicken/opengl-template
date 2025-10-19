#include "graphics/BackgroundRenderer.h"
#include "core/Settings.h"
#include "utils/Log.h"
#include "utils/ResourceManager.h"
#include <glad/glad.h>

BackgroundRenderer::BackgroundRenderer() = default;
BackgroundRenderer::~BackgroundRenderer() = default;

bool BackgroundRenderer::init(const Config &config) {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Always clear to fully transparent black. If the window is opaque, it will
  // just be black. If the window is transparent, this allows the desktop to
  // show through.
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // Always load the resources, since we always want to draw the shader.
  m_background_shader =
      ResourceManager::get_shader(config.background_shader_name);
  if (!m_background_shader) {
    Log::error("BackgroundRenderer failed to get shader '" +
               config.background_shader_name + "'");
    return false;
  }

  m_background_quad_mesh = ResourceManager::get_primitive("quad");
  if (!m_background_quad_mesh) {
    Log::error("BackgroundRenderer failed to get quad primitive.");
    return false;
  }

  Log::info("BackgroundRenderer initialized.");
  return true;
}

void BackgroundRenderer::update(float delta_time) {
  // Nothing to update
}

void BackgroundRenderer::draw(Scene &scene, unsigned int screen_width,
                              unsigned int screen_height) {
  // Always clear the color buffer.
  glClear(GL_COLOR_BUFFER_BIT);

  glDisable(GL_DEPTH_TEST);

  m_background_shader->use();
  m_background_quad_mesh->draw(*m_background_shader);

  glEnable(GL_DEPTH_TEST);
}
