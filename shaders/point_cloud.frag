#version 330 core
out vec4 FragColor;
in vec4 v_color;

void main()
{
    // Make the point a circle instead of a square
    if (length(gl_PointCoord - vec2(0.5)) > 0.5) {
        discard;
    }
    FragColor = v_color;
}
