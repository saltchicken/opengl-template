#include "graphics/CanvasRenderer.h"
#include "core/Settings.h"
#include "utils/Log.h"
#include "utils/ResourceManager.h"
#include <glad/glad.h>

CanvasRenderer::CanvasRenderer() = default;
CanvasRenderer::~CanvasRenderer() = default;

bool CanvasRenderer::init(const Config &config) {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Always clear to fully transparent black. If the window is opaque, it will
  // just be black. If the window is transparent, this allows the desktop to
  // show through.
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // Always load the resources, since we always want to draw the shader.
  m_canvas_shader = ResourceManager::get_shader(config.canvas_shader_name);
  if (!m_canvas_shader) {
    Log::error("CanvasRenderer failed to get shader '" +
               config.canvas_shader_name + "'");
    return false;
  }

  m_canvas_quad_mesh = ResourceManager::get_primitive("quad");
  if (!m_canvas_quad_mesh) {
    Log::error("CanvasRenderer failed to get quad primitive.");
    return false;
  }

  Log::info("CanvasRenderer initialized.");
  return true;
}

void CanvasRenderer::update(float delta_time) {
  // Nothing to update
}

void CanvasRenderer::draw(Scene &scene, unsigned int screen_width,
                          unsigned int screen_height) {
  // Always clear the color buffer.
  glClear(GL_COLOR_BUFFER_BIT);

  glDisable(GL_DEPTH_TEST);

  m_canvas_shader->use();
  m_canvas_quad_mesh->draw(*m_canvas_shader);

  glEnable(GL_DEPTH_TEST);
}
