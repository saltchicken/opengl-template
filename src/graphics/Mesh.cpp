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

// UPDATE: Destructor
Mesh::~Mesh() {
  if (m_vao != 0) {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    // Note: We do not delete the instance VBO/SSBO here.
    // Its lifetime is managed by the Component that creates it.
  }
}

// UPDATE: Copy Constructor
Mesh::Mesh(const Mesh &other)
    : vertices(other.vertices), indices(other.indices),
      textures(other.textures) {
  // We have copied the data, now we need to generate new OpenGL buffers for
  // this copy.
  setup_mesh();
  // Instancing setup is NOT copied. The new object must be configured for
  // instancing separately.
}

// UPDATE: Move Constructor
Mesh::Mesh(Mesh &&other) noexcept
    : vertices(std::move(other.vertices)), indices(std::move(other.indices)),
      textures(std::move(other.textures)), m_vao(other.m_vao),
      m_vbo(other.m_vbo), m_ebo(other.m_ebo),
      m_instance_count(other.m_instance_count) {
  other.m_vao = 0;
  other.m_vbo = 0;
  other.m_ebo = 0;
  other.m_instance_count = 0;
}

// UPDATE: Move Assignment
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
    m_vao = other.m_vao;
    m_vbo = other.m_vbo;
    m_ebo = other.m_ebo;
    m_instance_count = other.m_instance_count;

    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
    other.m_instance_count = 0;
  }
  return *this;
}

// Creates and configures the VAO, VBO, and EBO for the mesh.
void Mesh::setup_mesh() {
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);
  glGenBuffers(1, &m_ebo);

  glBindVertexArray(m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0],
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               &indices[0], GL_STATIC_DRAW);

  // Vertex Positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  // Vertex Normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Normal));
  // Vertex Texture Coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, TexCoords));

  glBindVertexArray(0);
}

// Renders the mesh.
void Mesh::draw(Shader &shader) {
  if (!textures.empty() && textures[0]) {
    shader.set_int("u_texture", 0);
    textures[0]->bind(0);
  }

  glBindVertexArray(m_vao);

  if (is_instanced()) {
    // Draw multiple instances of the mesh
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
                            GL_UNSIGNED_INT, 0, m_instance_count);
  } else {
    // Draw a single mesh
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
                   GL_UNSIGNED_INT, 0);
  }

  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

// --- NEW ---
bool Mesh::is_instanced() const { return m_instance_count > 0; }

// --- NEW ---
// This method doesn't create or upload data. It just configures the VAO
// to know that instance data will be provided from an SSBO in the vertex
// shader.
void Mesh::setup_instancing_ssbo(unsigned int ssbo, size_t instance_count) {
  if (instance_count == 0) {
    m_instance_count = 0;
    return;
  }
  m_instance_count = instance_count;
  // The key insight for SSBO-based instancing is that we don't need to
  // set up any more vertex attributes. The vertex shader will fetch the
  // instance data directly from the SSBO using `gl_InstanceID`.
  // Therefore, this function's only job is to store the instance count.
}
