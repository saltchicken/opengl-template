#pragma once
#include <string>
#include <memory>

class Shader {
public:
    // Constructor which builds the shader program from vertex and fragment shader files.
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    
    // Destructor to clean up the program.
    ~Shader();

    // Disable copying
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Enable moving
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    // Use/activate the shader program.
    void use() const;

    // (Optional) Add uniform setter functions here as your project grows.
    // void setInt(const std::string& name, int value);
    // void setFloat(const std::string& name, float value);

private:
    unsigned int m_id = 0; // The shader program ID

    // Private helper to check for compile/link errors.
    void checkCompileErrors(unsigned int shader, const std::string& type);
};
