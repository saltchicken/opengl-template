#include "graphics/renderers/PointCloudRenderer.h"
#include "core/Settings.h"
#include "graphics/PointCloud.h"
#include "graphics/Shader.h"
#include "graphics/fractal/AffineFractal.h"
#include "scene/CameraComponent.h"
#include "scene/PropertyAnimatorComponent.h"
#include "scene/SceneObject.h"
#include "utils/Log.h"
#include "utils/ResourceManager.h"
#include <glad/glad.h>

PointCloudRenderer::PointCloudRenderer() = default;
PointCloudRenderer::~PointCloudRenderer() = default;

bool PointCloudRenderer::init(const Config &config) {
  // 1. OpenGL settings
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_PROGRAM_POINT_SIZE); // Allows shaders to set point size
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  if (config.window_transparent) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  } else {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
  }

  // 2. Load shader
  m_shader = ResourceManager::get_shader(config.point_cloud_shader_name);
  if (!m_shader) {
    Log::error("PointCloudRenderer failed to get shader '" +
               config.point_cloud_shader_name + "'");
    return false;
  }

  // 3. Generate fractal data
  Log::info("Generating fractal point cloud...");
  AffineFractal fractal_generator;
  const int num_points = 500000;
  std::vector<Point> points = fractal_generator.generate(num_points);
  m_point_cloud = std::make_unique<PointCloud>(points);
  Log::info("Generated " + std::to_string(num_points) + " points.");

  // 4. Create internal scene objects
  // Camera
  m_camera_object = std::make_shared<SceneObject>();
  m_camera_object->transform->position = glm::vec3(0.0f, 0.0f, 6.0f);
  m_camera_object->add_component<CameraComponent>(45.0f, 0.1f, 100.0f);

  // Fractal object (for its transform)
  m_fractal_object = std::make_shared<SceneObject>();
  m_fractal_object->add_component<PropertyAnimatorComponent>(
      PropertyAnimatorComponent::TargetProperty::ROTATION,
      PropertyAnimatorComponent::RotationParams{
          glm::normalize(glm::vec3(0.0f, 1.0f, 0.2f)), 15.0f});

  Log::info("PointCloudRenderer initialized.");
  return true;
}

void PointCloudRenderer::update(float delta_time) {
  // Update the rotation of our fractal object
  if (m_fractal_object) {
    m_fractal_object->update(delta_time);
  }
}

void PointCloudRenderer::draw(Scene &scene, unsigned int screen_width,
                              unsigned int screen_height) {
  // We ignore the main application scene and use our own camera and object.

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (!m_camera_object || !m_fractal_object || !m_shader || !m_point_cloud) {
    return;
  }

  auto camera_comp = m_camera_object->get_component<CameraComponent>();
  if (!camera_comp) {
    return;
  }

  m_shader->use();

  // Set up matrices
  glm::mat4 view = camera_comp->get_view_matrix();
  float aspect_ratio =
      static_cast<float>(screen_width) / static_cast<float>(screen_height);
  glm::mat4 projection = camera_comp->get_projection_matrix(aspect_ratio);
  glm::mat4 model = m_fractal_object->transform->get_transform_matrix();

  m_shader->set_mat4("projection", projection);
  m_shader->set_mat4("view", view);
  m_shader->set_mat4("model", model);

  // Render the point cloud
  m_point_cloud->draw();
}
