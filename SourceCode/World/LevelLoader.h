#pragma once

#include "Renderer/RenderAssets.h"
#include "World/LevelMesh.h"
#include "World/MapData.h"

#include <entt/entt.hpp>
#include <glm/vec3.hpp>

#include <string>

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
        // The entity that draws the static geometry of the level; entt::null if the map has no world.
        entt::entity geometry = entt::null;

        PlayerStart playerStart;
        LevelMeshStatistics statistics;
    };

    // Creates the entities of a parsed map:
    //   - the world (worldspawn): its brushes become one mesh, stored in the mesh store under mapName, and one entity
    //     draws it, like the world is entity 0 in Quake. Collision data for the level will be built separately (0.2);
    //   - the player start (info_player_start): not an entity, its position and angle are returned.
    // Problems (no world, no player start) are logged; the level is then partly or fully empty.
    [[nodiscard]] LoadedLevel SpawnLevel(entt::registry& registry, Renderer::RenderAssets& assets, const MapData& map,
                                         const std::string& mapName);
}
