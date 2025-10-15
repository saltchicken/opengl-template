#include "Renderer.h"
#include "Shader.h" // Include the new header
#include <glad/glad.h>
#include <iostream>

Renderer::Renderer() = default;
Renderer::~Renderer() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    // m_shader is cleaned up automatically by unique_ptr
    std::cout << "Renderer resources cleaned up." << std::endl;
}

bool Renderer::init() {
    // 1. Create the shader program using our Shader class
    try {
        m_shader = std::make_unique<Shader>("shaders/shader.vert", "shaders/shader.frag");
    } catch (const std::exception& e) {
        std::cerr << "Failed to create shader: " << e.what() << std::endl;
        return false;
    }

    // 2. Set up vertex data (this part is unchanged)
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    std::cout << "Renderer initialized successfully." << std::endl;
    return true;
}

void Renderer::draw() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the shader and draw the triangle
    m_shader->use();
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
