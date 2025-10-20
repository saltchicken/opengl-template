#version 330 core
// ‼️ Attributes for the START fractal
layout (location = 0) in vec3 a_pos_start;
layout (location = 1) in vec4 a_color_start;

// ‼️ Attributes for the END fractal
layout (location = 2) in vec3 a_pos_end;
layout (location = 3) in vec4 a_color_end;

out vec4 v_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// ‼️ Uniform to control the animation (from 0.0 to 1.0)
uniform float u_interpolation_factor;

void main()
{
    // ‼️ Interpolate position and color
    vec3 final_pos = mix(a_pos_start, a_pos_end, u_interpolation_factor);
    v_color = mix(a_color_start, a_color_end, u_interpolation_factor);

    gl_Position = projection * view * model * vec4(final_pos, 1.0);

    float base_size = 40.0; // Try making this larger (e.g., 80.0) or smaller (e.g., 30.0)
    float min_size = 1.5;   // Increase this if distant points are too small

    gl_PointSize = max(min_size, base_size / gl_Position.w);
}
