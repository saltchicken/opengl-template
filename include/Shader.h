#pragma once
#include <string>
#include <memory>
#include <unordered_map> // For the cache

// Include GLM headers for vector and matrix types
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:

    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    // Disable copying
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Enable moving
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    // Use/activate the shader program.
    void use() const;

    void setBool(const std::string& name, bool value);
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec2(const std::string& name, const glm::vec2& value);
    void setVec3(const std::string& name, const glm::vec3& value);
    void setVec4(const std::string& name, const glm::vec4& value);
    void setMat4(const std::string& name, const glm::mat4& mat);

private:
    unsigned int m_id = 0; // The shader program ID

    mutable std::unordered_map<std::string, int> m_uniformLocationCache;
    int getUniformLocation(const std::string& name) const;

    // Private helper to check for compile/link errors.
    void checkCompileErrors(unsigned int shader, const std::string& type);
};
