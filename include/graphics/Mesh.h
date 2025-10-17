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

  // Constructor
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
       std::vector<std::shared_ptr<Texture>> textures);

  // Destructor to free GPU resources
  ~Mesh();

  // Disable copying to prevent double-deletion of GPU resources.
  // A mesh's data can be shared via std::shared_ptr, but the GPU resource
  // handle itself should have a single owner.
  Mesh(const Mesh &) = delete;
  Mesh &operator=(const Mesh &) = delete;

  // Enable moving for efficient resource transfer (e.g., from a factory)
  Mesh(Mesh &&other) noexcept;
  Mesh &operator=(Mesh &&other) noexcept;

  // Render the mesh
  void draw(Shader &shader);

private:
  // Render data - OpenGL handles
  unsigned int m_vao = 0;
  unsigned int m_vbo = 0;
  unsigned int m_ebo = 0;

  // Initializes all the buffer objects/arrays
  void setup_mesh();
};
