#pragma once

#include "Renderer/MeshData.h"
#include "World/MapData.h"

#include <glm/vec2.hpp>

#include <functional>
#include <string>

namespace Abomination::World
{
    // Numbers about the geometry of a level, for the debug overlay.
    struct LevelMeshStatistics
    {
        int brushCount = 0;
        int faceCount = 0;
        int triangleCount = 0;
    };

    // The geometry of all brushes of an entity (the world) ready to become one mesh, and the numbers about it.
    struct LevelMesh
    {
        Renderer::MeshData data;
        LevelMeshStatistics statistics;
    };

    // Gives the size in texels of the texture with this name (as the map writes it: "Episode1/Wall_MossyBrick").
    // Texture coordinates depend on it, and the texture files are known only to the caller (the texture store).
    using TextureSizeLookup = std::function<glm::ivec2(const std::string& textureName)>;

    // Builds the geometry of all brushes of an entity in the game's meters and axes: every face is built from its
    // planes (BuildBrushPolygons) and cut into triangles. Every vertex gets the normal of its face and its texture
    // coordinates (CalculateTextureCoordinates, with the texture size from getTextureSize).
    [[nodiscard]] LevelMesh BuildLevelMesh(const MapEntity& entity, const TextureSizeLookup& getTextureSize);
}
