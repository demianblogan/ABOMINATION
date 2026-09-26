#include "Gameplay/DemoCrates.h"

#include "Core/Name.h"
#include "Core/Transform.h"
#include "Core/TransformInterpolation.h"
#include "Gameplay/Spin.h"
#include "Renderer/MeshPrimitives.h"
#include "Renderer/MeshRenderer.h"

#include <glm/vec3.hpp>

#include <optional>
#include <string>
#include <utility>

namespace Abomination::Gameplay
{
    namespace
    {
        const std::string CubeMeshName = "Primitives/Cube";

        // Creates a crate entity: a name for the inspector, the "where" (Transform) and the "what to draw" (MeshRenderer).
        // Every crate gets the same handles, so the mesh and the texture exist once however many crates there are.
        entt::entity SpawnCrate(entt::registry& registry, const Renderer::MeshRenderer& crateLook, std::string name,
                                glm::vec3 position, float size)
        {
            const entt::entity crate = registry.create();
            registry.emplace<Core::Name>(crate, std::move(name));
            registry.emplace<Core::Transform>(crate, Core::Transform{.position = position, .scale = glm::vec3(size)});
            registry.emplace<Renderer::MeshRenderer>(crate, crateLook);

            return crate;
        }
    }

    void SpawnDemoCrates(entt::registry& registry, Renderer::RenderAssets& assets, glm::vec3 center)
    {
        // The cube is built only once: if it was already added, its handle is reused.
        const std::optional<Renderer::MeshHandle> existingMesh = assets.meshes.Find(CubeMeshName);
        const Renderer::MeshHandle cubeMesh =
            existingMesh.has_value() ? *existingMesh : assets.meshes.Add(CubeMeshName, Renderer::CreateCubeMeshData());

        const Renderer::MeshRenderer crateLook{
            .mesh = cubeMesh,
            .texture = assets.textures.Load("Textures/Crate.png"),
            .shaderProgram = assets.shaders.Load("Shaders/TexturedMesh"),
        };

        // A row of three crates standing on the floor next to each other, and one more on top of the middle one.
        // The cube mesh has size 1 and its origin in the middle, so a crate of size 1 stands on the floor when its center
        // is 0.5 above it; 1.1 between the centers leaves a small gap.
        SpawnCrate(registry, crateLook, "Crate", center + glm::vec3(-1.1f, 0.5f, 0.0f), 1.0f);
        SpawnCrate(registry, crateLook, "Crate", center + glm::vec3(0.0f, 0.5f, 0.0f), 1.0f);
        SpawnCrate(registry, crateLook, "Crate", center + glm::vec3(1.1f, 0.5f, 0.0f), 1.0f);
        SpawnCrate(registry, crateLook, "Stacked crate", center + glm::vec3(0.0f, 1.5f, 0.0f), 1.0f);

        // Two small crates floating above and turning: an entity is a crate that spins because it has one more
        // component, not because it is a different class. They move in ticks, so they are interpolated to be drawn
        // smoothly at any frame rate; the standing crates do not need it.
        const entt::entity leftSpinner =
            SpawnCrate(registry, crateLook, "Spinning crate", center + glm::vec3(-1.6f, 2.7f, 0.0f), 0.5f);
        registry.emplace<Spin>(leftSpinner, Spin{.axis = {0.6f, 1.0f, 0.0f}, .speed = 0.8f});
        Core::EnableInterpolation(registry, leftSpinner);

        const entt::entity rightSpinner =
            SpawnCrate(registry, crateLook, "Spinning crate", center + glm::vec3(1.6f, 2.7f, 0.0f), 0.5f);
        registry.emplace<Spin>(rightSpinner, Spin{.axis = {0.0f, 1.0f, 0.3f}, .speed = -1.5f});
        Core::EnableInterpolation(registry, rightSpinner);
    }
}
