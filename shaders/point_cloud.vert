#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec4 a_color;

out vec4 v_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
    v_color = a_color;
    // Make points smaller the further away they are, with a minimum size of 1.0
    gl_PointSize = max(1.0, 40.0 / gl_Position.w);
}
