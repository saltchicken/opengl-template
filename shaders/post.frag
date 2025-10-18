#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D u_raw_points_texture;
uniform sampler2D u_accumulation_texture;

uniform float u_denoise_factor;
uniform bool u_is_display_pass;

void main()
{
    if (u_is_display_pass) {
        // FINAL DISPLAY PASS: Just show the accumulated texture.
        FragColor = texture(u_accumulation_texture, TexCoords);
    } else {
        // ACCUMULATION PASS: Blend the previous frame with the new one.
        vec4 history = texture(u_accumulation_texture, TexCoords);
        vec4 current = texture(u_raw_points_texture, TexCoords);

        // Linearly interpolate between the history and the current frame's points.
        // A small denoise_factor means we keep more of the history, smoothing the image.
        FragColor = mix(history, current, u_denoise_factor);
    }
}
