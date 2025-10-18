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

  // 1. Define the affine transformations for a Sierpinski Tetrahedron
  m_transforms.resize(4);
  glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));

  m_transforms[0].matrix = glm::translate(scale_matrix, glm::vec3(1, 1, 1));
  m_transforms[1].matrix = glm::translate(scale_matrix, glm::vec3(-1, -1, 1));
  m_transforms[2].matrix = glm::translate(scale_matrix, glm::vec3(1, -1, -1));
  m_transforms[3].matrix = glm::translate(scale_matrix, glm::vec3(-1, 1, -1));

  // 2. Load the compute shader
  m_compute_shader = ResourceManager::load_compute_shader(
      "ifs_fractal", "shaders/ifs_fractal.comp");
  if (!m_compute_shader)
    return;

  // 3. Create and populate SSBO for the transformation rules
  glGenBuffers(1, &m_transforms_ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_transforms_ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               m_transforms.size() * sizeof(AffineTransform),
               m_transforms.data(), GL_STATIC_DRAW);

  // 4. Create SSBO for the output points
  glGenBuffers(1, &m_points_ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_points_ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, m_num_points * sizeof(glm::vec4),
               nullptr, GL_STATIC_DRAW);

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

  // 5. Configure the owner's mesh to be a point cloud
  m_owner->mesh->setup_as_point_cloud(m_num_points);
}

void IfsFractalComponent::update(float delta_time) {
  if (!m_compute_shader)
    return;

  m_compute_shader->use();

  // Pass a seed for the random number generator
  m_compute_shader->set_float("u_time", Time::get_total_time());
  m_compute_shader->set_int("u_iterations", m_iterations);

  // Bind the buffers
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_transforms_ssbo);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_points_ssbo);

  // Dispatch the compute shader
  const unsigned int work_group_size = 256;
  const unsigned int num_work_groups =
      (m_num_points + work_group_size - 1) / work_group_size;
  glDispatchCompute(num_work_groups, 1, 1);

  // Memory barrier to ensure compute shader finishes before rendering starts
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}
