#pragma once

#include "Renderer/RenderAssets.h"
#include "World/LevelMesh.h"
#include "World/MapData.h"

#include <entt/entt.hpp>
#include <glm/vec3.hpp>

#include <string>
#include <vector>

namespace Abomination::World
{
    // Where the player appears and where they look, in game coordinates (from the info_player_start entity).
    struct PlayerStart
    {
        // The position of the player's eyes.
        glm::vec3 eyePosition{0.0f};

        // Radians, like Gameplay::FreeFlyCamera::yaw.
        float yaw = 0.0f;
    };

    struct LoadedLevel
    {
        // The entities that draw the static geometry of the level, one per texture; empty if the map has no world.
        std::vector<entt::entity> geometry;

        PlayerStart playerStart;
        LevelMeshStatistics statistics;
    };

    // Creates the entities of a parsed map:
    //   - the world (worldspawn): its brushes become one mesh per texture, stored in the mesh store under
    //     "mapName#textureName", and one entity draws each of them (together they play the part of entity 0, the world,
    //     in Quake). Collision data for the level will be built separately (0.2);
    //   - the player start (info_player_start): not an entity, its position and angle are returned.
    // Problems (no world, no player start) are logged; the level is then partly or fully empty.
    [[nodiscard]] LoadedLevel SpawnLevel(entt::registry& registry, Renderer::RenderAssets& assets, const MapData& map,
                                         const std::string& mapName);

    // Destroys the entities of the level and removes every asset of the Level lifetime group (its textures and meshes)
    // from video memory. Global assets (shaders) stay. level is reset to an empty level.
    void UnloadLevel(entt::registry& registry, Renderer::RenderAssets& assets, LoadedLevel& level);
}
