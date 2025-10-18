#include "graphics/Shader.h"
#include <fstream>
#include <glad/glad.h>
#include <iostream>
#include <sstream>

namespace {
// Helper function to load a shader's source code from a file
std::string load_shader_source(const std::string &filepath) {
  std::ifstream shader_file;
  shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    shader_file.open(filepath);
    std::stringstream buffer;
    buffer << shader_file.rdbuf();
    shader_file.close();
    return buffer.str();
  } catch (std::ifstream::failure &e) {
    std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << filepath
              << std::endl;
    return "";
  }
}
} // namespace

// --- EXISTING CONSTRUCTOR FOR VERTEX/FRAGMENT SHADERS ---
Shader::Shader(const std::string &vertex_path,
               const std::string &fragment_path) {
  std::string vertex_code = load_shader_source(vertex_path);
  std::string fragment_code = load_shader_source(fragment_path);
  const char *v_shader_code = vertex_code.c_str();
  const char *f_shader_code = fragment_code.c_str();

  // 2. compile shaders
  unsigned int vertex, fragment;

  // vertex shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &v_shader_code, NULL);
  glCompileShader(vertex);
  check_compile_errors(vertex, "VERTEX");

  // fragment Shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &f_shader_code, NULL);
  glCompileShader(fragment);
  check_compile_errors(fragment, "FRAGMENT");

  // shader Program
  m_id = glCreateProgram();
  glAttachShader(m_id, vertex);
  glAttachShader(m_id, fragment);
  glLinkProgram(m_id);
  check_compile_errors(m_id, "PROGRAM");

  // delete the shaders as they're linked into our program now and no longer
  // necessary
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

// --- NEW CONSTRUCTOR FOR COMPUTE SHADERS ---
Shader::Shader(const std::string &compute_path) {
  std::string compute_code_str = load_shader_source(compute_path);
  const char *compute_code = compute_code_str.c_str();

  unsigned int compute_shader = glCreateShader(GL_COMPUTE_SHADER);
  glShaderSource(compute_shader, 1, &compute_code, NULL);
  glCompileShader(compute_shader);
  check_compile_errors(compute_shader, "COMPUTE");

  m_id = glCreateProgram();
  glAttachShader(m_id, compute_shader);
  glLinkProgram(m_id);
  check_compile_errors(m_id, "PROGRAM");

  glDeleteShader(compute_shader);
}

Shader::~Shader() {
  if (m_id != 0) {
    glDeleteProgram(m_id);
  }
}

Shader::Shader(Shader &&other) noexcept : m_id(other.m_id) {
  other.m_id = 0; // Prevent the moved-from object from deleting the program
}

Shader &Shader::operator=(Shader &&other) noexcept {
  if (this != &other) {
    if (m_id != 0) {
      glDeleteProgram(m_id);
    }
    m_id = other.m_id;
    other.m_id = 0;
  }
  return *this;
}

void Shader::use() const { glUseProgram(m_id); }

void Shader::check_compile_errors(unsigned int shader,
                                  const std::string &type) {
  int success;
  char infoLog[1024];
  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      std::cerr
          << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
          << infoLog
          << "\n -- --------------------------------------------------- -- "
          << std::endl;
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      std::cerr
          << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
          << infoLog
          << "\n -- --------------------------------------------------- -- "
          << std::endl;
    }
  }
}

// Uniform location caching
int Shader::get_uniform_location(const std::string &name) const {
  // Check if we have already cached the location
  if (m_uniform_location_cache.find(name) != m_uniform_location_cache.end()) {
    return m_uniform_location_cache[name];
  }

  // If not, query OpenGL for the location
  int location = glGetUniformLocation(m_id, name.c_str());
  if (location == -1) {
    // Log as debug because this can happen if GLSL compiler optimizes out a
    // uniform std::cerr << "Warning: uniform '" << name << "' not found!" <<
    // std::endl;
  }
  // Cache the location for future use
  m_uniform_location_cache[name] = location;
  return location;
}

// Uniform setter functions
void Shader::set_bool(const std::string &name, bool value) {
  glUniform1i(get_uniform_location(name), (int)value);
}
void Shader::set_int(const std::string &name, int value) {
  glUniform1i(get_uniform_location(name), value);
}
void Shader::set_float(const std::string &name, float value) {
  glUniform1f(get_uniform_location(name), value);
}
void Shader::set_vec2(const std::string &name, const glm::vec2 &value) {
  glUniform2fv(get_uniform_location(name), 1, &value[0]);
}
void Shader::set_vec3(const std::string &name, const glm::vec3 &value) {
  glUniform3fv(get_uniform_location(name), 1, &value[0]);
}
void Shader::set_vec4(const std::string &name, const glm::vec4 &value) {
  glUniform4fv(get_uniform_location(name), 1, &value[0]);
}
void Shader::set_mat4(const std::string &name, const glm::mat4 &mat) {
  glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, &mat[0][0]);
}
