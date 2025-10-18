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

  // Rule of 5 for proper resource management
  Mesh(const Mesh &other);
  Mesh &operator=(const Mesh &) = delete;
  Mesh(Mesh &&other) noexcept;
  Mesh &operator=(Mesh &&other) noexcept;

  // Render the mesh
  void draw(Shader &shader);

  // --- NEW INSTANCING METHOD ---
  // Sets up the mesh for instanced drawing using an SSBO.
  void setup_instancing_ssbo(unsigned int ssbo, size_t instance_count);
  bool is_instanced() const;

private:
  // Render data
  unsigned int m_vao = 0;
  unsigned int m_vbo = 0;
  unsigned int m_ebo = 0;

  // --- NEW INSTANCING DATA ---
  // We don't store the SSBO handle here, as it's managed by the component.
  // We just need to know how many instances to draw.
  size_t m_instance_count = 0;

  void setup_mesh();
};
