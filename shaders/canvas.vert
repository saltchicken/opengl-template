#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;

out vec2 v_tex_coord;

void main()
{
    // The quad primitive's vertices are from (-0.5, -0.5) to (0.5, 0.5).
    // We scale them by 2.0 to make them fill the screen in NDC (-1, -1) to (1, 1).
    // We set z to 0.0 and w to 1.0.
    gl_Position = vec4(aPos.xy * 2.0, 0.0, 1.0);
    v_tex_coord = aTexCoord;
}
