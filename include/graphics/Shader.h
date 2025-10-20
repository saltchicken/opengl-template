#pragma once
#include <string>
#include <unordered_map> // For the cache
#include <vector>

// Include GLM headers for vector and matrix types
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

enum class ShaderType {
  Graphics, // For a standard vertex + fragment pair
  Compute   // For a compute shader
};

class Shader {
public:
  Shader(ShaderType type, const std::vector<std::string> &paths);
  ~Shader();

  // Disable copying
  Shader(const Shader &) = delete;
  Shader &operator=(const Shader &) = delete;

  // Enable moving
  Shader(Shader &&other) noexcept;
  Shader &operator=(Shader &&other) noexcept;

  // Use/activate the shader program.
  void use() const;

  void set_bool(const std::string &name, bool value);
  void set_int(const std::string &name, int value);
  void set_float(const std::string &name, float value);
  void set_vec2(const std::string &name, const glm::vec2 &value);
  void set_vec3(const std::string &name, const glm::vec3 &value);
  void set_vec4(const std::string &name, const glm::vec4 &value);
  void set_mat4(const std::string &name, const glm::mat4 &mat);

private:
  unsigned int m_id = 0; // The shader program ID

  mutable std::unordered_map<std::string, int> m_uniform_location_cache;
  int get_uniform_location(const std::string &name) const;

  // Private helper to check for compile/link errors.
  void check_compile_errors(unsigned int shader, const std::string &type);
};
