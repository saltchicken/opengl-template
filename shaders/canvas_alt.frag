#version 330 core
out vec4 FragColor;
in vec2 v_tex_coord;

void main()
{
    // A fiery sunset gradient
    vec3 topColor = vec3(1.0, 0.2, 0.0);    // Orange
    vec3 bottomColor = vec3(0.5, 0.0, 0.5); // Purple

    FragColor = vec4(mix(bottomColor, topColor, v_tex_coord.y), 1.0);
}
