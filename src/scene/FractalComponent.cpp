#include "scene/FractalComponent.h"
#include "core/Time.h"
#include "graphics/Shader.h"
#include "scene/SceneObject.h"
#include "utils/Log.h"
#include "utils/ResourceManager.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

FractalComponent::FractalComponent(int recursion_depth,
                                   float sphere_scale_multiplier,
                                   float vibration_amplitude,
                                   float vibration_speed)
    : m_recursion_depth(recursion_depth),
      m_sphere_scale_multiplier(sphere_scale_multiplier),
      m_vibration_amplitude(vibration_amplitude),
      m_vibration_speed(vibration_speed) {}

FractalComponent::~FractalComponent() {
  // Clean up GPU resources
  if (m_base_matrices_ssbo != 0) {
    glDeleteBuffers(1, &m_base_matrices_ssbo);
  }
  if (m_dynamic_matrices_ssbo != 0) {
    glDeleteBuffers(1, &m_dynamic_matrices_ssbo);
  }
}

void FractalComponent::awake() {
  if (!m_owner || !m_owner->mesh) {
    Log::error(
        "FractalComponent must be attached to a SceneObject with a Mesh.");
    return;
  }

  // 1. Generate the base matrices on the CPU
  m_base_matrices.clear();
  generate_sponge(glm::vec3(0.0f), 3.0f, m_recursion_depth);
  Log::info("Generated " + std::to_string(m_base_matrices.size()) +
            " fractal instances.");

  // 2. Load the compute shader
  m_compute_shader = ResourceManager::load_compute_shader(
      "vibration", "shaders/vibration.comp");
  if (!m_compute_shader) {
    return;
  }

  // 3. Create and populate the SSBO for base matrices (read-only in shader)
  glGenBuffers(1, &m_base_matrices_ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_base_matrices_ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               m_base_matrices.size() * sizeof(glm::mat4),
               m_base_matrices.data(), GL_STATIC_DRAW);

  // 4. Create the SSBO for dynamic matrices (write-only in shader)
  // We allocate the memory but don't fill it with data from the CPU.
  glGenBuffers(1, &m_dynamic_matrices_ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_dynamic_matrices_ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               m_base_matrices.size() * sizeof(glm::mat4), nullptr,
               GL_DYNAMIC_DRAW);

  // 5. Unbind the buffer from the generic binding point
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

  // 6. Tell the mesh to use our dynamic SSBO for instanced rendering
  m_owner->mesh->setup_instancing_ssbo(m_dynamic_matrices_ssbo,
                                       m_base_matrices.size());
}

void FractalComponent::update(float delta_time) {
  if (!m_compute_shader || m_base_matrices.empty()) {
    return;
  }

  // 1. Activate the compute shader program
  m_compute_shader->use();

  // 2. Set uniforms for the compute shader
  m_compute_shader->set_float("u_time", Time::get_total_time());
  m_compute_shader->set_float("u_vibration_amplitude", m_vibration_amplitude);
  m_compute_shader->set_float("u_vibration_speed", m_vibration_speed);

  // 3. Bind the SSBOs to specific binding points
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_base_matrices_ssbo);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_dynamic_matrices_ssbo);

  // 4. Dispatch the compute shader
  // Calculate the number of workgroups needed. Each workgroup processes 256
  // items. The + 255 is a trick to ensure we have enough groups for all
  // instances (integer division ceiling).
  const unsigned int work_group_size = 256;
  const unsigned int num_work_groups =
      (m_base_matrices.size() + work_group_size - 1) / work_group_size;
  glDispatchCompute(num_work_groups, 1, 1);

  // 5. Add a memory barrier
  // This is crucial! It ensures that the compute shader has finished writing to
  // the SSBO before the rendering pipeline attempts to read from it.
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void FractalComponent::generate_sponge(const glm::vec3 &position, float scale,
                                       int depth) {
  if (depth == 0) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(scale * m_sphere_scale_multiplier));
    m_base_matrices.push_back(model);
    return;
  }

  float new_scale = scale / 3.0f;
  for (int x = -1; x <= 1; ++x) {
    for (int y = -1; y <= 1; ++y) {
      for (int z = -1; z <= 1; ++z) {
        if (std::abs(x) + std::abs(y) + std::abs(z) >= 2) {
          glm::vec3 new_pos = position + glm::vec3(x, y, z) * new_scale;
          generate_sponge(new_pos, new_scale, depth - 1);
        }
      }
    }
  }
}
