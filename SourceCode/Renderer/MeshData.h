#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <cstdint>
#include <vector>

namespace Abomination::Renderer
{
    // One vertex as it lies in the vertex buffer: 3 floats of position, then 2 floats of texture coordinates.
    //   bytes:  0              12         20
    //           | x | y | z | u | v |
    // Normals are added with lighting (0.5).
    struct MeshVertex
    {
        glm::vec3 position{0.0f};
        glm::vec2 texCoord{0.0f};
    };

    // The geometry of a mesh in ordinary memory, before it is uploaded to the GPU: vertices and the indices that make
    // triangles of them (every 3 indices are one triangle, counter-clockwise when looked at from its front side).
    // Built by code (MeshPrimitives) now, loaded from model files later (0.3). Needs no OpenGL, so it can be tested.
    struct MeshData
    {
        std::vector<MeshVertex> vertices;
        std::vector<std::uint32_t> indices;
    };
}
