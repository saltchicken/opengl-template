#include "scene/FractalComponent.h"
#include "scene/SceneObject.h"
#include "utils/Log.h"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

FractalComponent::FractalComponent(int recursion_depth)
    : m_recursion_depth(recursion_depth) {}

void FractalComponent::awake() {
  if (!m_owner || !m_owner->mesh) {
    Log::error(
        "FractalComponent must be attached to a SceneObject with a Mesh.");
    return;
  }

  m_matrices.clear();
  // Start the recursive generation
  generate_sponge(glm::vec3(0.0f), 3.0f, m_recursion_depth);

  Log::info("Generated " + std::to_string(m_matrices.size()) +
            " fractal instances.");

  // Send the generated data to the mesh for GPU upload
  m_owner->mesh->setup_instancing(m_matrices);
}

void FractalComponent::generate_sponge(const glm::vec3 &position, float scale,
                                       int depth) {
  // Base case: If we've reached the desired depth, create the final transform
  if (depth == 0) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(scale));
    m_matrices.push_back(model);
    return;
  }

  // Recursive step: Generate 20 smaller sponges
  float new_scale = scale / 3.0f;
  for (int x = -1; x <= 1; ++x) {
    for (int y = -1; y <= 1; ++y) {
      for (int z = -1; z <= 1; ++z) {
        // The Menger sponge rule: skip the center and face-centers
        if (std::abs(x) + std::abs(y) + std::abs(z) >= 2) {
          glm::vec3 new_pos = position + glm::vec3(x, y, z) * new_scale;
          generate_sponge(new_pos, new_scale, depth - 1);
        }
      }
    }
  }
}
