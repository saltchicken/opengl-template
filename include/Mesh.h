#pragma once

#include <string>
#include <vector>

#include "Shader.h"
#include <glm/glm.hpp>

struct Vertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;
};

// NOTE: For now, this is a placeholder. We'll need a real Texture class later.
struct Texture {
  unsigned int id;
  std::string type; // e.g., "texture_diffuse", "texture_specular"
};

class Mesh {
public:
  // Mesh data
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures; // We'll use this later

  // Constructor
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
       std::vector<Texture> textures);

  // Render the mesh
  void draw(Shader &shader);

private:
  // Render data
  unsigned int m_vao = 0;
  unsigned int m_vbo = 0;
  unsigned int m_ebo = 0;

  // Initializes all the buffer objects/arrays
  void setup_mesh();
};
