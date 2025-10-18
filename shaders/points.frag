#version 430 core

out vec4 FragColor;

in float v_point_size;
uniform float u_max_point_size;
uniform float u_point_alpha;

void main() {
    if (v_point_size > u_max_point_size) {
        discard;
    }

    // Calculate the distance from the center of the point (0.5, 0.5)
    float dist = distance(gl_PointCoord, vec2(0.5));

    // If the distance is greater than the radius (0.5), discard the pixel.
    if (dist > 0.5) {
        discard;
    }

    // A simple, bright color for the fractal points
    FragColor = vec4(0.0, 1.0, 0.8, u_point_alpha);
}
