#include "ResourceManager.h"
#include "PrimitiveFactory.h"
#include <iostream>

// Instantiate static variables
std::unordered_map<std::string, std::shared_ptr<Shader>>
    ResourceManager::m_shaders;
std::unordered_map<std::string, std::shared_ptr<Texture>>
    ResourceManager::m_textures;
std::unordered_map<std::string, std::shared_ptr<Mesh>>
    ResourceManager::m_meshes;

std::shared_ptr<Shader>
ResourceManager::load_shader(const std::string &name,
                             const std::string &v_shader_file,
                             const std::string &f_shader_file) {
  if (m_shaders.find(name) == m_shaders.end()) {
    try {
      auto shader = std::make_shared<Shader>(v_shader_file, f_shader_file);
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
      m_meshes[name] = PrimitiveFactory::create_quad();

    }
    // TODO: Add other primitives here...
    else {
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
