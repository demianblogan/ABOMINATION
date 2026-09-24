#version 460 core

// Runs once for every vertex. Takes the vertex from the buffer (through the vertex array)
// and tells OpenGL where on the screen it is.

// Vertex attributes: read from the vertex buffer; the locations match GLVertexArray::SetFloatAttribute calls.
layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec3 aColor;

// Passed on to the fragment shader. For every pixel between the vertices OpenGL blends the values
// of the three vertices of the triangle (interpolation), which produces the smooth color gradient.
layout(location = 0) out vec3 Color;

void main()
{
    Color = aColor;

    // gl_Position is the built-in output: the position in clip space. With w = 1 it equals the normalized device
    // coordinates, where the window spans from -1 to 1 on both axes, (0, 0) is its center and +Y points up.
    gl_Position = vec4(aPosition, 0.0, 1.0);
}
