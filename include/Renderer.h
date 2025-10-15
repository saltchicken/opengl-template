#pragma once

class Renderer {
public:
    Renderer();
    ~Renderer();

    // Creates shaders, buffers, and sets up vertex data.
    bool init();

    // Clears the screen and draws the scene.
    void draw();

private:
    // OpenGL handles for our triangle
    unsigned int m_shaderProgram = 0;
    unsigned int m_vao = 0; // Vertex Array Object
    unsigned int m_vbo = 0; // Vertex Buffer Object
};
