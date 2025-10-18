#version 330 core

// Per-vertex attributes from the VBO
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal; // We're not using it, but it's good practice
layout (location = 2) in vec2 aTexCoord;

// Per-instance attribute from a separate VBO
layout (location = 3) in mat4 aInstanceMatrix;

// Outputs to fragment shader
out vec2 TexCoord;

// Uniforms
uniform mat4 model;      // The parent transform
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Apply the parent model transform *before* the instance-specific one
    gl_Position = projection * view * model * aInstanceMatrix * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
