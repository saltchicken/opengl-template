#include "scene/IfsFractalComponent.h"
#include "core/Time.h"
#include "graphics/Shader.h"
#include "scene/SceneObject.h"
#include "utils/Log.h"
#include "utils/ResourceManager.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

IfsFractalComponent::IfsFractalComponent(size_t num_points, int iterations)
    : m_num_points(num_points), m_iterations(iterations) {}

IfsFractalComponent::~IfsFractalComponent() {
  if (m_transforms_ssbo != 0) {
    glDeleteBuffers(1, &m_transforms_ssbo);
  }
  if (m_points_ssbo != 0) {
    glDeleteBuffers(1, &m_points_ssbo);
  }
}

void IfsFractalComponent::awake() {
  if (!m_owner || !m_owner->mesh) {
    Log::error("IfsFractalComponent must be attached to an object with a "
               "(dummy) mesh.");
    return;
  }

  // --- NEW: A different set of base transforms that works well with the
  // spherical variation ---
  m_transforms.resize(1);
  glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f));
  glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(137.5f),
                                 glm::vec3(0.1, 0.5, 0.8));
  glm::mat4 translate =
      glm::translate(glm::mat4(1.0f), glm::vec3(0.1, 0.0, 0.0));
  m_transforms[0].matrix = translate * rotate * scale;

  m_compute_shader = ResourceManager::load_compute_shader(
      "ifs_fractal", "shaders/ifs_fractal.comp");
  if (!m_compute_shader)
    return;

  glGenBuffers(1, &m_transforms_ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_transforms_ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               m_transforms.size() * sizeof(AffineTransform),
               m_transforms.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &m_points_ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_points_ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, m_num_points * sizeof(glm::vec4),
               nullptr, GL_STATIC_DRAW);

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

  m_owner->mesh->setup_as_point_cloud(m_num_points);
}

void IfsFractalComponent::update(float delta_time) {
  if (!m_compute_shader)
    return;

  m_compute_shader->use();

  m_compute_shader->set_float("u_time", Time::get_total_time());
  m_compute_shader->set_int("u_iterations", m_iterations);

  m_compute_shader->set_float("u_twist_strength", 2.0f);
  m_compute_shader->set_float("u_spiral_strength", 0.99f);

  // --- NEW: Set the spherical variation strength uniform ---
  // 1.0 applies the effect fully. 0.0 would disable it.
  m_compute_shader->set_float("u_spherical_strength", 1.0f);

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_transforms_ssbo);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_points_ssbo);

  const unsigned int work_group_size = 256;
  const unsigned int num_work_groups =
      (m_num_points + work_group_size - 1) / work_group_size;
  glDispatchCompute(num_work_groups, 1, 1);

  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}
