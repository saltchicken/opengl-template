#include "scene/PhyllotaxisComponent.h"
#include "scene/SceneObject.h"
#include "utils/Log.h"
#include <glm/gtc/matrix_transform.hpp>
#include <numbers> // For std::numbers::pi_v<float> in C++20

PhyllotaxisComponent::PhyllotaxisComponent(int num_points, float radius)
    : m_num_points(num_points), m_radius(radius) {}

void PhyllotaxisComponent::awake() {
  if (!m_owner || !m_owner->mesh) {
    Log::error(
        "PhyllotaxisComponent must be attached to a SceneObject with a Mesh.");
    return;
  }

  m_matrices.clear();

  // The "golden angle" in radians. This is the magic number for the spiral.
  const float golden_angle =
      std::numbers::pi_v<float> * (3.0f - std::sqrt(5.0f));

  for (int i = 0; i < m_num_points; ++i) {
    // Distribute y uniformly from -1 to 1
    float y = 1.0f - (static_cast<float>(i) / (m_num_points - 1)) * 2.0f;

    // Calculate the radius of the circle at this y-height
    float radius_at_y = std::sqrt(1.0f - y * y);

    // Calculate the angle for this point
    float theta = golden_angle * i;

    // Calculate the x and z coordinates
    float x = std::cos(theta) * radius_at_y;
    float z = std::sin(theta) * radius_at_y;

    // Final position is on the surface of a sphere of the given radius
    glm::vec3 position = glm::vec3(x, y, z) * m_radius;

    // Make the spheres smaller toward the poles for a more organic look
    float sphere_scale = 0.08f * radius_at_y;

    // Create the model matrix for this instance
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(sphere_scale));

    m_matrices.push_back(model);
  }

  Log::info("Generated " + std::to_string(m_matrices.size()) +
            " phyllotaxis instances.");

  // Send the matrix data to the mesh for GPU upload
  m_owner->mesh->setup_instancing(m_matrices);
}
