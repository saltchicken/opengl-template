#include "graphics/fractal/AffineFractal.h"
#include <glm/gtc/matrix_transform.hpp> // ‼️ For glm::rotate
#include <random>

AffineFractal::AffineFractal() {
  // Constructor is now empty, setup is called by generate
}

void AffineFractal::setup_sierpinski_tetrahedron() {
  // ... (this function's content is unchanged)
  m_transforms.clear();
  const float scale = 0.5f;
  glm::mat3 scale_matrix = glm::mat3(scale);
  float s = 1.5f;
  glm::vec3 p1(s * 1.0f, s * 1.0f, s * 1.0f);
  glm::vec3 p2(s * -1.0f, s * -1.0f, s * 1.0f);
  glm::vec3 p3(s * -1.0f, s * 1.0f, s * -1.0f);
  glm::vec3 p4(s * 1.0f, s * -1.0f, s * -1.0f);
  m_transforms.push_back(
      {scale_matrix, p1 * scale, glm::vec4(1.0f, 0.2f, 0.2f, 1.0f)});
  m_transforms.push_back(
      {scale_matrix, p2 * scale, glm::vec4(0.2f, 1.0f, 0.2f, 1.0f)});
  m_transforms.push_back(
      {scale_matrix, p3 * scale, glm::vec4(0.2f, 0.2f, 1.0f, 1.0f)});
  m_transforms.push_back(
      {scale_matrix, p4 * scale, glm::vec4(1.0f, 1.0f, 0.2f, 1.0f)});
}

// ‼️ Create a new function for the second fractal
void AffineFractal::setup_twisted_cube() {
  m_transforms.clear();

  const float scale = 0.5f;
  glm::mat3 scale_matrix = glm::mat3(scale);

  // Define a slight rotation for a twisting effect
  glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f),
                                          glm::vec3(0.5f, 1.0f, 0.0f));

  // Scale and rotate matrix
  glm::mat3 transform_matrix = glm::mat3(rotation_matrix) * scale_matrix;

  // Define 8 corners of a cube
  float c = 1.8f;
  std::vector<glm::vec3> corners;
  for (int i = 0; i < 8; ++i) {
    corners.push_back(
        glm::vec3((i & 1) ? c : -c, (i & 2) ? c : -c, (i & 4) ? c : -c));
  }

  std::vector<glm::vec4> colors = {
      {1, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 1, 1}, {1, 1, 0, 1},
      {1, 0, 1, 1}, {0, 1, 1, 1}, {1, 1, 1, 1}, {0.5, 0.5, 0.5, 1}};

  for (int i = 0; i < 8; ++i) {
    m_transforms.push_back({transform_matrix, corners[i] * scale, colors[i]});
  }
}

// ‼️ Update generate to select the fractal type
std::vector<Point> AffineFractal::generate(FractalType type, int num_points) {
  switch (type) {
  case FractalType::SierpinskiTetrahedron:
    setup_sierpinski_tetrahedron();
    break;
  case FractalType::TwistedCube:
    setup_twisted_cube();
    break;
  }

  std::vector<Point> points;
  // ... (rest of the function is unchanged)
  points.reserve(num_points);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, (int)m_transforms.size() - 1);
  glm::vec3 current_point(0.0f, 0.0f, 0.0f);
  int warmup_iterations = 20;
  for (int i = 0; i < num_points + warmup_iterations; ++i) {
    int choice = distrib(gen);
    const auto &transform = m_transforms[choice];
    current_point = (transform.matrix * current_point) + transform.translation;
    if (i >= warmup_iterations) {
      points.push_back({current_point, transform.color});
    }
  }
  return points;
}
