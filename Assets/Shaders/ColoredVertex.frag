#version 460 core

// Runs once for every pixel (fragment) covered by the triangle and decides its color.

// The blended color from the vertex shader: same location, same type.
layout(location = 0) in vec3 Color;

// Written into color attachment 0 of the framebuffer: the back buffer of the window.
layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = vec4(Color, 1.0);
}
