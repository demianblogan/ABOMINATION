#version 460 core

// Paints every line of the wireframe in one color. Textures and shading are left out on purpose: a line one pixel
// wide that takes its color from a texture shows whatever texel it happens to cross, so the lines of textured meshes
// would be patchy and hard to see.

layout(location = 0) out vec4 FragColor;

// Light gray, clearly visible on the dark background.
const vec3 LineColor = vec3(0.85, 0.85, 0.85);

void main()
{
    FragColor = vec4(LineColor, 1.0);
}
