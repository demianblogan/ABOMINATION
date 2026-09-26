#version 460 core

// Places the vertices like the other shaders and passes nothing else on: the wireframe needs only positions.
// Used for every mesh when the Renderer window of the debug overlay switches to Wireframe.

layout(location = 0) in vec3 aPosition;

layout(location = 0) uniform mat4 uniModel;
layout(location = 1) uniform mat4 uniView;
layout(location = 2) uniform mat4 uniProjection;

void main()
{
    gl_Position = uniProjection * uniView * uniModel * vec4(aPosition, 1.0);
}
