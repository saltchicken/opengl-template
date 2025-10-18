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
  // Mesh data
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<std::shared_ptr<Texture>> textures;

  // Constructor & Destructor
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
       std::vector<std::shared_ptr<Texture>> textures);
  ~Mesh();

  // Rule of 5
  Mesh(const Mesh &) = delete;
  Mesh &operator=(const Mesh &) = delete;
  Mesh(Mesh &&other) noexcept;
  Mesh &operator=(Mesh &&other) noexcept;

  // Render the mesh
  void draw(Shader &shader);

  // --- NEW INSTANCING METHODS ---
  void setup_instancing(const std::vector<glm::mat4> &matrices);
  bool is_instanced() const;

private:
  // Render data
  unsigned int m_vao = 0;
  unsigned int m_vbo = 0;
  unsigned int m_ebo = 0;

  // --- NEW INSTANCING DATA ---
  unsigned int m_instance_vbo = 0; // VBO for instance model matrices
  size_t m_instance_count = 0;

  void setup_mesh();
};
