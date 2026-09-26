#pragma once

#include "Renderer/Camera.h"
#include "Renderer/RenderAssets.h"

#include <entt/entt.hpp>

namespace Abomination::Renderer
{
    // The render system: draws every entity that has a Core::Transform and a MeshRenderer, as seen through the camera.
    // aspectRatio is the width of the drawable area divided by its height (must be above 0).
    // Only reads the registry: drawing never changes the game.
    void DrawMeshes(const entt::registry& registry, const Camera& camera, float aspectRatio, const RenderAssets& assets);
}
