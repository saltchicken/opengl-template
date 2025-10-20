#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <vector>

// A simple struct to hold our point data for the VBO.
struct Point {
  glm::vec3 position;
  glm::vec4 color;
};

class PointCloud {
public:
  PointCloud(const std::vector<Point> &points);
  ~PointCloud();

  // Rule of 5 for proper resource management
  PointCloud(const PointCloud &) = delete;
  PointCloud &operator=(const PointCloud &) = delete;
  PointCloud(PointCloud &&other) noexcept;
  PointCloud &operator=(PointCloud &&other) noexcept;

  // Renders the points.
  void draw();

private:
  void setup_buffers(const std::vector<Point> &points);

  unsigned int m_vao = 0;
  unsigned int m_vbo = 0;
  int m_point_count = 0;
};
