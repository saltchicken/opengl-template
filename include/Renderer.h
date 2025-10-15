#pragma once
#include <memory>

class Shader; // Forward declaration

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool init();
    void draw();

private:
    std::unique_ptr<Shader> m_shader; // Manages the shader's lifetime
    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
};
