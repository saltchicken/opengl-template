#pragma once

#include "graphics/Shader.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class Texture;

struct Vertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;
};

class Mesh {
public:
  // --- NEW: RenderMode enum ---
  enum class RenderMode { TRIANGLES, POINTS };

  // Mesh data
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<std::shared_ptr<Texture>> textures;
  RenderMode render_mode = RenderMode::TRIANGLES; // Default to triangles

  // Constructor & Destructor
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
       std::vector<std::shared_ptr<Texture>> textures);
  ~Mesh();

  // Rule of 5 for proper resource management
  Mesh(const Mesh &other);
  Mesh &operator=(const Mesh &) = delete;
  Mesh(Mesh &&other) noexcept;
  Mesh &operator=(Mesh &&other) noexcept;

  // Render the mesh
  void draw(Shader &shader);

  void setup_instancing_ssbo(unsigned int ssbo, size_t instance_count);
  bool is_instanced() const;

  // --- NEW: Method for point cloud setup ---
  void setup_as_point_cloud(size_t point_count);
  size_t get_point_count() const;

private:
  // Render data
  unsigned int m_vao = 0;
  unsigned int m_vbo = 0;
  unsigned int m_ebo = 0;

  size_t m_instance_count = 0;
  size_t m_point_count = 0; // NEW

  void setup_mesh();
};
