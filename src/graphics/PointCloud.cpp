#include "graphics/PointCloud.h"
#include <glad/glad.h>
#include <utility> // for std::move

PointCloud::PointCloud(const std::vector<Point> &points) {
  m_point_count = static_cast<int>(points.size());
  setup_buffers(points);
}

PointCloud::~PointCloud() {
  if (m_vao != 0) {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
  }
}

PointCloud::PointCloud(PointCloud &&other) noexcept
    : m_vao(other.m_vao), m_vbo(other.m_vbo),
      m_point_count(other.m_point_count) {
  other.m_vao = 0;
  other.m_vbo = 0;
  other.m_point_count = 0;
}

PointCloud &PointCloud::operator=(PointCloud &&other) noexcept {
  if (this != &other) {
    if (m_vao != 0) {
      glDeleteVertexArrays(1, &m_vao);
      glDeleteBuffers(1, &m_vbo);
    }
    m_vao = other.m_vao;
    m_vbo = other.m_vbo;
    m_point_count = other.m_point_count;
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_point_count = 0;
  }
  return *this;
}

void PointCloud::setup_buffers(const std::vector<Point> &points) {
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);

  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Point), &points[0],
               GL_STATIC_DRAW);

  // Position attribute (location = 0)
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void *)0);
  // Color attribute (location = 1)
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Point),
                        (void *)offsetof(Point, color));

  glBindVertexArray(0);
}

void PointCloud::draw() {
  glBindVertexArray(m_vao);
  glDrawArrays(GL_POINTS, 0, m_point_count);
  glBindVertexArray(0);
}
