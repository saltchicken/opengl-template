#include "utils/ResourceManager.h"
#include <iostream>

// Instantiate static variables
std::unordered_map<std::string, std::shared_ptr<Shader>>
    ResourceManager::m_shaders;
std::unordered_map<std::string, std::shared_ptr<Texture>>
    ResourceManager::m_textures;
std::unordered_map<std::string, std::shared_ptr<Mesh>>
    ResourceManager::m_meshes;

std::shared_ptr<Shader>
ResourceManager::load_shader(const std::string &name, ShaderType type,
                             const std::vector<std::string> &paths) {
  if (m_shaders.find(name) == m_shaders.end()) {
    try {
      auto shader = std::make_shared<Shader>(type, paths);
      m_shaders[name] = shader;
      return shader;
    } catch (const std::exception &e) {
      std::cerr << "Failed to load shader '" << name << "': " << e.what()
                << std::endl;
      return nullptr;
    }
  }
  return m_shaders[name];
}

std::shared_ptr<Shader> ResourceManager::get_shader(const std::string &name) {
  if (m_shaders.find(name) != m_shaders.end()) {
    return m_shaders[name];
  }
  std::cerr << "Shader '" << name << "' not found." << std::endl;
  return nullptr;
}

std::shared_ptr<Texture>
ResourceManager::load_texture(const std::string &name,
                              const std::string &file) {
  if (m_textures.find(name) == m_textures.end()) {
    auto texture = std::make_shared<Texture>(file);
    // Check if texture was loaded successfully (m_id != 0)
    if (texture && texture->get_id() != 0) {
      m_textures[name] = texture;
      return texture;
    }
    std::cerr << "Failed to load texture '" << name << "' from file: " << file
              << std::endl;
    return nullptr;
  }
  return m_textures[name];
}

std::shared_ptr<Texture> ResourceManager::get_texture(const std::string &name) {
  if (m_textures.find(name) != m_textures.end()) {
    return m_textures[name];
  }
  std::cerr << "Texture '" << name << "' not found." << std::endl;
  return nullptr;
}

std::shared_ptr<Mesh> ResourceManager::get_primitive(const std::string &name) {
  if (m_meshes.find(name) == m_meshes.end()) {
    if (name == "quad") {
      m_meshes[name] = create_quad();
    } else if (name == "cube") {
      m_meshes[name] = create_cube();
    } else if (name == "sphere") {
      m_meshes[name] = create_sphere();
    } else {
      std::cerr << "Primitive '" << name << "' not recognized." << std::endl;
      return nullptr;
    }
  }
  return m_meshes[name];
}

void ResourceManager::clear() {
  // The smart pointers will handle the deletion of the OpenGL objects
  m_shaders.clear();
  m_textures.clear();
  m_meshes.clear();
}

std::shared_ptr<Mesh> ResourceManager::create_quad() {
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
  std::vector<std::shared_ptr<Texture>> textures;
  return std::make_shared<Mesh>(vertices, indices, textures);
}

std::shared_ptr<Mesh> ResourceManager::create_cube() {
  std::vector<Vertex> vertices = {
      // positions           // normals           // texture coords
      // Back face (-Z)
      {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // 0
      {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},  // 1
      {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},   // 2
      {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},  // 3
      // Front face (+Z)
      {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // 4
      {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // 5
      {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},   // 6
      {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // 7
      // Left face (-X)
      {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},   // 8
      {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},  // 9
      {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // 10
      {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // 11
      // Right face (+X)
      {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},   // 12
      {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},  // 13
      {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // 14
      {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // 15
      // Bottom face (-Y)
      {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, // 16
      {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},  // 17
      {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},   // 18
      {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},  // 19
      // Top face (+Y)
      {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}, // 20
      {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},  // 21
      {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},   // 22
      {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}   // 23
  };
  std::vector<unsigned int> indices = {// Back face
                                       0, 1, 2, 2, 3, 0,
                                       // Front face
                                       4, 5, 6, 6, 7, 4,
                                       // Left face
                                       8, 9, 10, 10, 11, 8,
                                       // Right face
                                       12, 13, 14, 14, 15, 12,
                                       // Bottom face
                                       16, 17, 18, 18, 19, 16,
                                       // Top face
                                       20, 21, 22, 22, 23, 20};
  std::vector<std::shared_ptr<Texture>> textures;
  return std::make_shared<Mesh>(vertices, indices, textures);
}

std::shared_ptr<Mesh> ResourceManager::create_sphere() {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  const unsigned int X_SEGMENTS = 64;
  const unsigned int Y_SEGMENTS = 64;
  const float PI = 3.14159265359f;
  for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
    for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
      float x_segment = (float)x / (float)X_SEGMENTS;
      float y_segment = (float)y / (float)Y_SEGMENTS;
      float x_pos = std::cos(x_segment * 2.0f * PI) * std::sin(y_segment * PI);
      float y_pos = std::cos(y_segment * PI);
      float z_pos = std::sin(x_segment * 2.0f * PI) * std::sin(y_segment * PI);
      Vertex vertex;
      vertex.Position = {x_pos, y_pos, z_pos};
      vertex.TexCoords = {x_segment, y_segment};
      vertex.Normal = {x_pos, y_pos, z_pos};
      vertices.push_back(vertex);
    }
  }
  for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
    for (unsigned int x = 0; x < X_SEGMENTS; ++x) {
      indices.push_back(y * (X_SEGMENTS + 1) + x);
      indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
      indices.push_back(y * (X_SEGMENTS + 1) + x + 1);
      indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
      indices.push_back((y + 1) * (X_SEGMENTS + 1) + x + 1);
      indices.push_back(y * (X_SEGMENTS + 1) + x + 1);
    }
  }
  std::vector<std::shared_ptr<Texture>> textures;
  return std::make_shared<Mesh>(vertices, indices, textures);
}
