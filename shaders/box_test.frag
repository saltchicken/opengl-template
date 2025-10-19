#version 330 core
out vec4 FragColor;

// This comes from the vertex shader, ranging from (0,0) to (1,1) across the screen.
in vec2 v_tex_coord;

void main()
{
    // Define the boundaries of our box.
    // We want it to be centered, so we check from 0.25 to 0.75 on both axes.
    // This makes the box take up the central 50% of the screen.
    bool in_box_x = v_tex_coord.x > 0.25 && v_tex_coord.x < 0.75;
    bool in_box_y = v_tex_coord.y > 0.25 && v_tex_coord.y < 0.75;

    if (in_box_x && in_box_y) {
        // Inside the box: output opaque green.
        FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    } else {
        // Outside the box: output transparent black.
        // The alpha of 0.0 is what makes it see-through.
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}

