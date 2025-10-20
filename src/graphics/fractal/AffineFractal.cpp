#include "graphics/fractal/AffineFractal.h"
#include <random>

AffineFractal::AffineFractal() { setup_sierpinski_tetrahedron(); }

void AffineFractal::setup_sierpinski_tetrahedron() {
  m_transforms.clear();

  const float scale = 0.5f;
  glm::mat3 scale_matrix = glm::mat3(scale);

  // Vertices of a regular tetrahedron, scaled for better viewing
  float s = 1.5f;
  glm::vec3 p1(s * 1.0f, s * 1.0f, s * 1.0f);
  glm::vec3 p2(s * -1.0f, s * -1.0f, s * 1.0f);
  glm::vec3 p3(s * -1.0f, s * 1.0f, s * -1.0f);
  glm::vec3 p4(s * 1.0f, s * -1.0f, s * -1.0f);

  // Each transform scales the point by 0.5 and translates it halfway to a
  // vertex
  m_transforms.push_back(
      {scale_matrix, p1 * scale, glm::vec4(1.0f, 0.2f, 0.2f, 1.0f)}); // Red
  m_transforms.push_back(
      {scale_matrix, p2 * scale, glm::vec4(0.2f, 1.0f, 0.2f, 1.0f)}); // Green
  m_transforms.push_back(
      {scale_matrix, p3 * scale, glm::vec4(0.2f, 0.2f, 1.0f, 1.0f)}); // Blue
  m_transforms.push_back(
      {scale_matrix, p4 * scale, glm::vec4(1.0f, 1.0f, 0.2f, 1.0f)}); // Yellow
}

std::vector<Point> AffineFractal::generate(int num_points) {
  std::vector<Point> points;
  points.reserve(num_points);

  // C++ random number generation
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, (int)m_transforms.size() - 1);

  glm::vec3 current_point(0.0f, 0.0f, 0.0f);

  // The first few iterations are often discarded to let the point
  // settle onto the fractal's "attractor".
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
