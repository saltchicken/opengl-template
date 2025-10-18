#include "graphics/Mesh.h"
#include "graphics/Texture.h"
#include <glad/glad.h>
#include <utility> // For std::move

// Constructor remains the same
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
           std::vector<std::shared_ptr<Texture>> textures)
    : vertices(std::move(vertices)), indices(std::move(indices)),
      textures(std::move(textures)) {
  setup_mesh();
}

Mesh::~Mesh() {
  if (m_vao != 0) {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
  }
}

Mesh::Mesh(const Mesh &other)
    : vertices(other.vertices), indices(other.indices),
      textures(other.textures) {
  setup_mesh();
}

Mesh::Mesh(Mesh &&other) noexcept
    : vertices(std::move(other.vertices)), indices(std::move(other.indices)),
      textures(std::move(other.textures)), m_vao(other.m_vao),
      m_vbo(other.m_vbo), m_ebo(other.m_ebo),
      m_instance_count(other.m_instance_count),
      m_point_count(other.m_point_count), // move new member
      render_mode(other.render_mode) {    // move new member
  other.m_vao = 0;
  other.m_vbo = 0;
  other.m_ebo = 0;
  other.m_instance_count = 0;
  other.m_point_count = 0;
}

Mesh &Mesh::operator=(Mesh &&other) noexcept {
  if (this != &other) {
    if (m_vao != 0) {
      glDeleteVertexArrays(1, &m_vao);
      glDeleteBuffers(1, &m_vbo);
      glDeleteBuffers(1, &m_ebo);
    }
    vertices = std::move(other.vertices);
    indices = std::move(other.indices);
    textures = std::move(other.textures);
    render_mode = other.render_mode;
    m_vao = other.m_vao;
    m_vbo = other.m_vbo;
    m_ebo = other.m_ebo;
    m_instance_count = other.m_instance_count;
    m_point_count = other.m_point_count;
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
    other.m_instance_count = 0;
    other.m_point_count = 0;
  }
  return *this;
}

// --- REVISED FUNCTION ---
void Mesh::setup_mesh() {
  // Every mesh, even a point cloud container, needs a VAO.
  if (m_vao == 0) {
    glGenVertexArrays(1, &m_vao);
  }

  // Only set up VBOs, EBOs, and attribute pointers if we actually have vertex
  // data.
  if (!vertices.empty() && !indices.empty()) {
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
  }
}

void Mesh::draw(Shader &shader) {
  if (!textures.empty() && textures[0]) {
    shader.set_int("u_texture", 0);
    textures[0]->bind(0);
  }

  glBindVertexArray(m_vao);

  // --- NEW: Branch based on render mode ---
  if (render_mode == RenderMode::POINTS) {
    glDrawArrays(GL_POINTS, 0, m_point_count);
  } else if (is_instanced()) {
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
                            GL_UNSIGNED_INT, 0, m_instance_count);
  } else {
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
                   GL_UNSIGNED_INT, 0);
  }

  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

bool Mesh::is_instanced() const { return m_instance_count > 0; }

void Mesh::setup_instancing_ssbo(unsigned int ssbo, size_t instance_count) {
  m_instance_count = instance_count;
  render_mode = RenderMode::TRIANGLES;
}

void Mesh::setup_as_point_cloud(size_t point_count) {
  m_point_count = point_count;
  render_mode = RenderMode::POINTS;
  // We just need a VAO for point rendering, which we create in setup_mesh
  if (m_vao == 0) {
    setup_mesh();
  }
}

size_t Mesh::get_point_count() const { return m_point_count; }
