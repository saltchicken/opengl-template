#include "scene/IfsFractalComponent.h"
#include "core/Time.h"
#include "graphics/Shader.h"
#include "scene/SceneObject.h"
#include "utils/Log.h"
#include "utils/ResourceManager.h"
#include <cmath> // For sin
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

  // --- DEFINE TWO FRACTALS ---
  // For easy interpolation, both fractals must have the same number of
  // transforms.
  m_num_base_transforms = 4;

  std::vector<AffineTransform> transforms_A(m_num_base_transforms);
  std::vector<AffineTransform> transforms_B(m_num_base_transforms);

  // Fractal A: "Spherical Spiral" (Based on your previous fractal)
  // We'll pad it to 4 transforms by repeating the rule.
  glm::mat4 scale_A = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f));
  glm::mat4 rotate_A = glm::rotate(glm::mat4(1.0f), glm::radians(137.5f),
                                   glm::vec3(0.1, 0.5, 0.8));
  glm::mat4 translate_A =
      glm::translate(glm::mat4(1.0f), glm::vec3(0.1, 0.0, 0.0));
  transforms_A[0].matrix = transforms_A[1].matrix = transforms_A[2].matrix =
      transforms_A[3].matrix = translate_A * rotate_A * scale_A;

  // Fractal B: "Sierpinski Tetrahedron"
  glm::mat4 scale_B = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
  transforms_B[0].matrix = glm::translate(scale_B, glm::vec3(1, 1, 1));
  transforms_B[1].matrix = glm::translate(scale_B, glm::vec3(-1, -1, 1));
  transforms_B[2].matrix = glm::translate(scale_B, glm::vec3(1, -1, -1));
  transforms_B[3].matrix = glm::translate(scale_B, glm::vec3(-1, 1, -1));

  // Combine both sets of transforms into one large vector for the GPU
  std::vector<AffineTransform> all_transforms;
  all_transforms.insert(all_transforms.end(), transforms_A.begin(),
                        transforms_A.end());
  all_transforms.insert(all_transforms.end(), transforms_B.begin(),
                        transforms_B.end());
  // all_transforms now contains transforms_A followed by transforms_B

  // --- SETUP SHADER AND BUFFERS ---
  m_compute_shader = ResourceManager::load_compute_shader(
      "ifs_fractal", "shaders/ifs_fractal.comp");
  if (!m_compute_shader)
    return;

  glGenBuffers(1, &m_transforms_ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_transforms_ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               all_transforms.size() * sizeof(AffineTransform),
               all_transforms.data(), GL_STATIC_DRAW);

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

  // --- CALCULATE INTERPOLATION FACTOR ---
  // Use a sine wave to smoothly transition from 0.0 to 1.0 and back
  float animation_speed = 0.2f;
  float interpolation_factor =
      (sin(Time::get_total_time() * animation_speed) + 1.0f) / 2.0f;

  // --- PASS UNIFORMS TO SHADER ---
  m_compute_shader->set_float("u_time", Time::get_total_time());
  m_compute_shader->set_int("u_iterations", m_iterations);
  m_compute_shader->set_int("u_num_base_transforms", m_num_base_transforms);
  m_compute_shader->set_float("u_interpolation_factor", interpolation_factor);

  // Parameters for Fractal A (Spherical Spiral)
  m_compute_shader->set_float("u_twist_A", 2.0f);
  m_compute_shader->set_float("u_spiral_A", 0.99f);
  m_compute_shader->set_float("u_spherical_A", 1.0f);

  // Parameters for Fractal B (Sierpinski). These values disable the non-linear
  // effects.
  m_compute_shader->set_float("u_twist_B", 0.0f);
  m_compute_shader->set_float("u_spiral_B", 1.0f);
  m_compute_shader->set_float("u_spherical_B", 0.0f);

  // --- DISPATCH COMPUTE SHADER ---
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_transforms_ssbo);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_points_ssbo);
  const unsigned int work_group_size = 256;
  const unsigned int num_work_groups =
      (m_num_points + work_group_size - 1) / work_group_size;
  glDispatchCompute(num_work_groups, 1, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}
