#version 460 core

// Runs once for every vertex: passes the texture coordinates on and places the vertex on the screen.

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTexCoord;

// Texture coordinates (UV) of this vertex: (0, 0) is the bottom-left corner of the texture, (1, 1) the top-right.
// For every pixel between the vertices they are interpolated, so each pixel knows which point of the texture it shows.
layout(location = 0) out vec2 TexCoord;

void main()
{
    TexCoord = aTexCoord;

    // Normalized device coordinates for now; matrices come with the 3D cube.
    gl_Position = vec4(aPosition, 0.0, 1.0);
}
