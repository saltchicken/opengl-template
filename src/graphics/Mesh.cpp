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
    if (m_instance_vbo != 0) {
      glDeleteBuffers(1, &m_instance_vbo);
    }
  }
}

// UPDATE: Move Constructor
Mesh::Mesh(Mesh &&other) noexcept
    : vertices(std::move(other.vertices)), indices(std::move(other.indices)),
      textures(std::move(other.textures)), m_vao(other.m_vao),
      m_vbo(other.m_vbo), m_ebo(other.m_ebo),
      m_instance_vbo(other.m_instance_vbo),
      m_instance_count(other.m_instance_count) {
  other.m_vao = 0;
  other.m_vbo = 0;
  other.m_ebo = 0;
  other.m_instance_vbo = 0;
  other.m_instance_count = 0;
}

// UPDATE: Move Assignment
Mesh &Mesh::operator=(Mesh &&other) noexcept {
  if (this != &other) {
    if (m_vao != 0) {
      glDeleteVertexArrays(1, &m_vao);
      glDeleteBuffers(1, &m_vbo);
      glDeleteBuffers(1, &m_ebo);
      if (m_instance_vbo != 0) {
        glDeleteBuffers(1, &m_instance_vbo);
      }
    }
    vertices = std::move(other.vertices);
    indices = std::move(other.indices);
    textures = std::move(other.textures);
    m_vao = other.m_vao;
    m_vbo = other.m_vbo;
    m_ebo = other.m_ebo;
    m_instance_vbo = other.m_instance_vbo;
    m_instance_count = other.m_instance_count;

    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
    other.m_instance_vbo = 0;
    other.m_instance_count = 0;
  }
  return *this;
}

// Creates and configures the VAO, VBO, and EBO for the mesh.
void Mesh::setup_mesh() {
  // 1. Create buffers/arrays
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);
  glGenBuffers(1, &m_ebo);

  // 2. Bind the Vertex Array Object first, then bind and set vertex buffer(s),
  // and then configure vertex attributes(s).
  glBindVertexArray(m_vao);

  // 3. Copy our vertices array into a vertex buffer for OpenGL to use
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0],
               GL_STATIC_DRAW);

  // 4. Copy our index array in a element buffer for OpenGL to use
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               &indices[0], GL_STATIC_DRAW);

  // 5. Set the vertex attribute pointers
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

  // Unbind the VAO
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

// --- NEW ---
bool Mesh::is_instanced() const {
  return m_instance_vbo != 0 && m_instance_count > 0;
}

// --- NEW ---
void Mesh::setup_instancing(const std::vector<glm::mat4> &matrices) {
  if (matrices.empty()) {
    m_instance_count = 0;
    return;
  }
  m_instance_count = matrices.size();

  // Create the VBO if it doesn't exist yet
  if (m_instance_vbo == 0) {
    glGenBuffers(1, &m_instance_vbo);
  }

  // Bind the VAO first
  glBindVertexArray(m_vao);

  // Upload the instance matrix data to the new VBO
  glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);
  glBufferData(GL_ARRAY_BUFFER, m_instance_count * sizeof(glm::mat4),
               matrices.data(), GL_STATIC_DRAW);

  // Set up vertex attribute pointers for the instance matrix.
  // A mat4 is equivalent to 4 vec4s.
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)0);
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                        (void *)(sizeof(glm::vec4)));
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                        (void *)(2 * sizeof(glm::vec4)));
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                        (void *)(3 * sizeof(glm::vec4)));

  // This is the crucial part: tell OpenGL that these attributes are
  // per-instance
  glVertexAttribDivisor(3, 1);
  glVertexAttribDivisor(4, 1);
  glVertexAttribDivisor(5, 1);
  glVertexAttribDivisor(6, 1);

  // Unbind
  glBindVertexArray(0);
}
