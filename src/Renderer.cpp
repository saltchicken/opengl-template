#include "Renderer.h"
#include "Shader.h"
#include <glad/glad.h>
#include <iostream>

Renderer::Renderer() = default;
Renderer::~Renderer() = default;

bool Renderer::init() {
  // 1. Create the shader program using our Shader class
  try {
    m_shader =
        std::make_unique<Shader>("shaders/shader.vert", "shaders/shader.frag");
  } catch (const std::exception &e) {
    std::cerr << "Failed to create shader: " << e.what() << std::endl;
    return false;
  }

  std::vector<Vertex> vertices = {
      // positions          // normals          // texture coords
      {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},   // top right
      {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // bottom right
      {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom left
      {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}   // top left
  };

  std::vector<unsigned int> indices = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };

  std::vector<Texture> textures; // Empty for now

  // 3. Create a Mesh object and add it to our list of meshes
  m_meshes.emplace_back(vertices, indices, textures);

  std::cout << "Renderer initialized successfully." << std::endl;
  return true;
}

void Renderer::update(float delta_time) {
  // std::cout << delta_time << std::endl;
}

void Renderer::draw() {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Use the shader and draw the triangle
  m_shader->use();

  for (auto &mesh : m_meshes) {
    mesh.draw(*m_shader);
  }
}
