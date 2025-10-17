#include "Mesh.h"
#include "Texture.h"
#include <glad/glad.h>
#include <utility> // For std::move

// Constructor: Initializes the mesh with data and sets up GPU buffers.
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
           std::vector<std::shared_ptr<Texture>> textures)
    : vertices(std::move(vertices)), indices(std::move(indices)),
      textures(std::move(textures)) {
  // Now that we have all the required data, set up the vertex buffers and
  // attribute pointers.
  setup_mesh();
}

// Destructor: Cleans up the GPU resources when the mesh object is destroyed.
Mesh::~Mesh() {
  // Only try to delete if the handles are valid (not 0)
  if (m_vao != 0) {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
  }
}

// Move Constructor: Transfers ownership of GPU resources from another mesh.
Mesh::Mesh(Mesh &&other) noexcept
    : vertices(std::move(other.vertices)), indices(std::move(other.indices)),
      textures(std::move(other.textures)), m_vao(other.m_vao),
      m_vbo(other.m_vbo), m_ebo(other.m_ebo) {
  // Prevent the moved-from object's destructor from freeing the buffers by
  // setting its handles to 0. This is crucial for preventing double-deletion.
  other.m_vao = 0;
  other.m_vbo = 0;
  other.m_ebo = 0;
}

// Move Assignment Operator: Transfers ownership from another mesh.
Mesh &Mesh::operator=(Mesh &&other) noexcept {
  // 1. Check for self-assignment
  if (this != &other) {
    // 2. Free existing resources of the current object
    if (m_vao != 0) {
      glDeleteVertexArrays(1, &m_vao);
      glDeleteBuffers(1, &m_vbo);
      glDeleteBuffers(1, &m_ebo);
    }

    // 3. Move data and resource handles from the other object
    vertices = std::move(other.vertices);
    indices = std::move(other.indices);
    textures = std::move(other.textures);
    m_vao = other.m_vao;
    m_vbo = other.m_vbo;
    m_ebo = other.m_ebo;

    // 4. Prevent the other object's destructor from freeing the resources
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
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
  // Bind the first texture if it exists.
  // A more advanced system would loop through all textures.
  if (!textures.empty() && textures[0]) {
    shader.set_int("u_texture", 0); // Tell shader to use texture unit 0
    textures[0]->bind(0);
  }

  // Bind the VAO and draw the elements
  glBindVertexArray(m_vao);
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
                 GL_UNSIGNED_INT, 0);

  // Unbind the VAO to be clean
  glBindVertexArray(0);

  // Unbind texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
}
