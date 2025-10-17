#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include <memory>
#include <string>
#include <unordered_map>

class ResourceManager {
public:
  // This class is not meant to be instantiated.
  ResourceManager() = delete;

  // Shaders
  static std::shared_ptr<Shader> load_shader(const std::string &name,
                                             const std::string &v_shader_file,
                                             const std::string &f_shader_file);
  static std::shared_ptr<Shader> get_shader(const std::string &name);

  // Textures
  static std::shared_ptr<Texture> load_texture(const std::string &name,
                                               const std::string &file);
  static std::shared_ptr<Texture> get_texture(const std::string &name);

  // Meshes
  static std::shared_ptr<Mesh> get_primitive(const std::string &name);

  // Clears all stored resources
  static void clear();

private:
  static std::shared_ptr<Mesh> create_quad();

  static std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
  static std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
  static std::unordered_map<std::string, std::shared_ptr<Mesh>> m_meshes;
};
