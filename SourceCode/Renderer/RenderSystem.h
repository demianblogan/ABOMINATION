#pragma once

#include "Renderer/Camera.h"
#include "Renderer/RenderAssets.h"

#include <entt/entt.hpp>

namespace Abomination::Renderer
{
    // The render system: draws every entity that has a Core::Transform and a MeshRenderer, as seen through the camera.
    // aspectRatio is the width of the drawable area divided by its height (must be above 0).
    // Entities with a Core::PreviousTransform are drawn at fraction interpolationFactor of the way from their previous
    // to their current transform (see Core/TransformInterpolation.h); the others at their current transform.
    // Only reads the registry: drawing never changes the game.
    void DrawMeshes(const entt::registry& registry, const Camera& camera, float aspectRatio, float interpolationFactor,
                    const RenderAssets& assets);
}
