#include "Renderer.h"
#include "Camera.h"
#include "Shader.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Renderer::Renderer() = default;
Renderer::~Renderer() = default;

bool Renderer::init() {
  glEnable(GL_DEPTH_TEST);
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

void Renderer::draw(Camera &camera, unsigned int screen_width,
                    unsigned int screen_height) {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Use the shader and draw the triangle
  m_shader->use();

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = camera.GetViewMatrix();

  glm::mat4 projection =
      glm::perspective(glm::radians(camera.Fov),
                       (float)screen_width / screen_height, 0.1f, 100.0f);

  m_shader->set_mat4("model", model);
  m_shader->set_mat4("projection", projection);
  m_shader->set_mat4("view", view);

  for (auto &mesh : m_meshes) {
    mesh.draw(*m_shader);
  }
}
