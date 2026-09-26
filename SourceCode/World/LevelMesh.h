#pragma once

#include "Renderer/MeshData.h"
#include "World/MapData.h"

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

    // Builds the geometry of all brushes of an entity in the game's meters and axes: every face is built from its
    // planes (BuildBrushPolygons) and cut into triangles. Every vertex gets the normal of its face. Texture coordinates
    // stay 0 until textures are drawn on the level.
    [[nodiscard]] LevelMesh BuildLevelMesh(const MapEntity& entity);
}
