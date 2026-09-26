#version 460 core

// Places the vertices like TexturedMesh.vert and passes on the direction each surface faces.
// Used for surfaces without textures (the level until textures are drawn on it).

layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec3 aNormal;

layout(location = 0) uniform mat4 uniModel;
layout(location = 1) uniform mat4 uniView;
layout(location = 2) uniform mat4 uniProjection;

layout(location = 0) out vec3 Normal;

void main()
{
    // The normal is turned with the model (mat3 keeps the rotation and drops the translation of the model matrix).
    // This is exact for rotations and uniform scaling, which is all the game uses.
    Normal = mat3(uniModel) * aNormal;

    gl_Position = uniProjection * uniView * uniModel * vec4(aPosition, 1.0);
}
