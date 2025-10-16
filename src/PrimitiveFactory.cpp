#include "PrimitiveFactory.h"

std::shared_ptr<Mesh> PrimitiveFactory::create_quad() {
  std::vector<Vertex> vertices = {
      // positions            // normals           // texture coords
      {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom left
      {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // bottom right
      {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},   // top right
      {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}   // top left
  };

  std::vector<unsigned int> indices = {
      0, 1, 2, // first triangle
      2, 3, 0  // second triangle
  };

  // For primitives, we can start with an empty texture vector.
  std::vector<Texture> textures;

  return std::make_shared<Mesh>(vertices, indices, textures);
}
