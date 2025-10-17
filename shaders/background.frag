#version 330 core
out vec4 FragColor;

in vec2 v_tex_coord;

void main()
{
    vec3 topColor = vec3(0.1, 0.2, 0.4);    // Dark blue
    vec3 bottomColor = vec3(0.5, 0.7, 1.0); // Light sky blue

    // Linearly interpolate between the bottom and top colors based on the y-coordinate.
    FragColor = vec4(mix(bottomColor, topColor, v_tex_coord.y), 1.0);
}
