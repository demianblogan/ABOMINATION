#pragma once

#include "Renderer/RenderAssets.h"

#include <entt/entt.hpp>

namespace Abomination::Gameplay
{
    // Creates the entities of the temporary demo level: a few crates, some of them spinning. Replaced by levels loaded
    // from TrenchBroom maps (0.2, map geometry). The assets are loaded through the stores, so all crates share one mesh,
    // one texture and one shader program.
    void SpawnDemoLevel(entt::registry& registry, Renderer::RenderAssets& assets);
}
