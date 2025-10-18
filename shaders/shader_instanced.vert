#version 430 core

// Per-vertex attributes from the VBO
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

// Outputs to fragment shader
out vec2 TexCoord;

// Uniforms
uniform mat4 model; // The parent transform of the entire fractal object
uniform mat4 view;
uniform mat4 projection;

// --- FIX: Ensure we read from the same binding point the compute shader writes to ---
// The compute shader writes to binding = 1, so we read from binding = 1.
layout(std430, binding = 1) readonly buffer DynamicMatrices {
    mat4 matrices[];
} dynamic_matrices;


void main()
{
    // Fetch the instance-specific matrix from the SSBO using the instance ID
    mat4 instanceMatrix = dynamic_matrices.matrices[gl_InstanceID];

    // Apply the parent model transform *before* the instance-specific one
    gl_Position = projection * view * model * instanceMatrix * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
