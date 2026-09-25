#version 460 core

// Runs once for every vertex: moves it from the model's own coordinates to the screen
// and passes the texture coordinates on.

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

// Uniforms: the same for all vertices of one draw call, set from C++ by GLShaderProgram::SetUniform.
//   Model      - places the model in the world: its position, rotation and size.
//   View       - moves the world so that the camera is at the origin looking along -Z.
//   Projection - applies perspective: distant points move closer to the center, so distant things look smaller.
layout(location = 0) uniform mat4 uniModel;
layout(location = 1) uniform mat4 uniView;
layout(location = 2) uniform mat4 uniProjection;

layout(location = 0) out vec2 TexCoord;

void main()
{
    TexCoord = aTexCoord;

    // Matrices are applied from right to left: first Model, then View, then Projection.
    // w = 1.0 marks a point (not a direction), so the translation part of the matrices affects it.
    gl_Position = uniProjection * uniView * uniModel * vec4(aPosition, 1.0);
}
