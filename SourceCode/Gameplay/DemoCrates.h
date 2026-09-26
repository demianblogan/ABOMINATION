#pragma once

#include "Renderer/RenderAssets.h"

#include <entt/entt.hpp>
#include <glm/vec3.hpp>

namespace Abomination::Gameplay
{
    // Creates a few crates, some of them spinning, standing around center (a point on the floor in game coordinates).
    // Temporary objects to look at until levels have their own (pickups, enemies). The assets are loaded through the
    // stores, so all crates share one mesh, one texture and one shader program.
    void SpawnDemoCrates(entt::registry& registry, Renderer::RenderAssets& assets, glm::vec3 center);
}
