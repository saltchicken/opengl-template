#version 430 core

out vec4 FragColor;

void main() {
    // --- FIX: Output a color with alpha for blending ---
    FragColor = vec4(0.9, 1.0, 0.8, 0.2); // Low alpha for a ghostly effect
}
