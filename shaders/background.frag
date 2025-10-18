#version 330 core
out vec4 FragColor;
in vec2 v_tex_coord;

uniform sampler1D u_fft_texture; // Our FFT data

void main()
{
    // Sample the FFT texture based on the horizontal position
    // We sample a little bit into the texture to get more bass/mid response
    float fft_val = texture(u_fft_texture, v_tex_coord.x * 0.4).r;

    vec3 topColor = vec3(0.1, 0.2, 0.4);      // Dark blue
    vec3 bottomColor = vec3(0.5, 0.7, 1.0);   // Light sky blue

    // A "pulse" color that gets brighter with audio
    vec3 pulseColor = vec3(0.9, 0.2, 0.5) * fft_val * 2.0;

    // Mix the original gradient with the pulse color
    vec3 gradient = mix(bottomColor, topColor, v_tex_coord.y);
    vec3 finalColor = gradient + pulseColor;

    FragColor = vec4(finalColor, 1.0);
}
