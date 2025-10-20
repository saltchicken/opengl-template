#include "graphics/renderers/PointCloudRenderer.h"
#include "core/Settings.h"
#include "core/Time.h" // ‼️ Include for animation
#include "graphics/PointCloud.h"
#include "graphics/Shader.h"
#include "graphics/fractal/AffineFractal.h"
#include "scene/CameraComponent.h"
#include "scene/PropertyAnimatorComponent.h"
#include "scene/SceneObject.h"
#include "utils/Log.h"
#include "utils/ResourceManager.h"
#include <cmath> // ‼️ For sin()
#include <glad/glad.h>

PointCloudRenderer::PointCloudRenderer() = default;
// ‼️ Add a destructor to clean up OpenGL objects
PointCloudRenderer::~PointCloudRenderer() {
  if (m_vao != 0) {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo_start);
    glDeleteBuffers(1, &m_vbo_end);
  }
}

bool PointCloudRenderer::init(const Config &config) {
  // 1. OpenGL settings
  // ... (unchanged)
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_PROGRAM_POINT_SIZE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  if (config.window_transparent) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  } else {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  }

  // 2. Load shader
  m_shader = ResourceManager::get_shader(config.point_cloud_shader_name);
  // ... (unchanged error checking)

  // 3. ‼️ Generate BOTH sets of fractal data
  Log::info("Generating fractal point clouds...");
  AffineFractal fractal_generator;
  const int num_points = 500000;
  m_point_count = num_points;

  std::vector<Point> points_start = fractal_generator.generate(
      AffineFractal::FractalType::SierpinskiTetrahedron, num_points);
  std::vector<Point> points_end = fractal_generator.generate(
      AffineFractal::FractalType::TwistedCube, num_points);
  Log::info("Generated " + std::to_string(num_points * 2) +
            " total points for interpolation.");

  // 4. ‼️ Set up VAO and VBOs
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo_start);
  glGenBuffers(1, &m_vbo_end);

  glBindVertexArray(m_vao);

  // VBO for the starting fractal
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo_start);
  glBufferData(GL_ARRAY_BUFFER, points_start.size() * sizeof(Point),
               points_start.data(), GL_STATIC_DRAW);
  // Start Position attribute
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point),
                        (void *)offsetof(Point, position));
  // Start Color attribute
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Point),
                        (void *)offsetof(Point, color));

  // VBO for the ending fractal
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo_end);
  glBufferData(GL_ARRAY_BUFFER, points_end.size() * sizeof(Point),
               points_end.data(), GL_STATIC_DRAW);
  // End Position attribute
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Point),
                        (void *)offsetof(Point, position));
  // End Color attribute
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Point),
                        (void *)offsetof(Point, color));

  glBindVertexArray(0);

  // 5. Create internal scene objects
  // ... (this part is unchanged)
  m_camera_object = std::make_shared<SceneObject>();
  m_camera_object->transform->position = glm::vec3(0.0f, 0.0f, 6.0f);
  m_camera_object->add_component<CameraComponent>(45.0f, 0.1f, 100.0f);
  m_fractal_object = std::make_shared<SceneObject>();
  m_fractal_object->add_component<PropertyAnimatorComponent>(
      PropertyAnimatorComponent::TargetProperty::ROTATION,
      PropertyAnimatorComponent::RotationParams{
          glm::normalize(glm::vec3(0.0f, 1.0f, 0.2f)), 15.0f});

  Log::info("PointCloudRenderer initialized for interpolation.");
  return true;
}

void PointCloudRenderer::update(float delta_time) {
  // Update the rotation of our fractal object
  if (m_fractal_object) {
    m_fractal_object->update(delta_time);
  }
  // ‼️ Update the interpolation factor using a sine wave for a smooth
  // back-and-forth animation
  m_interpolation_factor =
      (sin(static_cast<float>(Time::get_total_time()) * 0.5f) + 1.0f) / 2.0f;
}

void PointCloudRenderer::draw(Scene &scene, unsigned int screen_width,
                              unsigned int screen_height) {
  // ... (setup code is mostly the same)
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (!m_camera_object || !m_fractal_object || !m_shader || m_vao == 0) {
    return;
  }
  // ... camera and matrix setup is the same
  m_shader->use();
  glm::mat4 view =
      m_camera_object->get_component<CameraComponent>()->get_view_matrix();
  float aspect_ratio =
      static_cast<float>(screen_width) / static_cast<float>(screen_height);
  glm::mat4 projection =
      m_camera_object->get_component<CameraComponent>()->get_projection_matrix(
          aspect_ratio);
  glm::mat4 model = m_fractal_object->transform->get_transform_matrix();
  m_shader->set_mat4("projection", projection);
  m_shader->set_mat4("view", view);
  m_shader->set_mat4("model", model);

  // ‼️ Set the interpolation uniform in the shader
  m_shader->set_float("u_interpolation_factor", m_interpolation_factor);

  // ‼️ Render using the raw VAO handle
  glBindVertexArray(m_vao);
  glDrawArrays(GL_POINTS, 0, m_point_count);
  glBindVertexArray(0);
}
