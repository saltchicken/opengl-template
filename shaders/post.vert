#version 330 core

// Match the Vertex struct layout from your C++ code
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;  // We receive this but don't use it
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoord;
    
    // The quad primitive's vertices are from (-0.5, -0.5) to (0.5, 0.5).
    // We scale them by 2.0 to make them fill the screen in NDC (-1 to 1).
    gl_Position = vec4(aPos.xy * 2.0, 0.0, 1.0);
}
