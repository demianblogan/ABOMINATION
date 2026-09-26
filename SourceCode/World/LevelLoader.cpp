#include "World/LevelLoader.h"

#include "Core/Log.h"
#include "Core/Name.h"
#include "Core/Transform.h"
#include "Renderer/MeshRenderer.h"
#include "World/MapCoordinates.h"

#include <charconv>
#include <optional>
#include <system_error>

namespace Abomination::World
{
    using Core::LogCategory;
    using Core::LogLevel;

    namespace
    {
        // The eyes of the Quake player are 22 units above the origin of info_player_start (the center of its box).
        constexpr double EyeHeightAboveOrigin = 22.0;

        const MapEntity* FindEntity(const MapData& map, const std::string& className)
        {
            for (const MapEntity& entity : map.entities)
                if (const std::string* entityClass = FindProperty(entity, "classname"); entityClass != nullptr)
                    if (*entityClass == className)
                        return &entity;

            return nullptr;
        }

        PlayerStart ReadPlayerStart(const MapData& map)
        {
            PlayerStart playerStart;

            const MapEntity* entity = FindEntity(map, "info_player_start");
            if (entity == nullptr)
            {
                Core::Log::Write(LogCategory::World, LogLevel::Warning, "The map has no info_player_start");

                return playerStart;
            }

            if (const std::string* origin = FindProperty(*entity, "origin"); origin != nullptr)
            {
                if (const std::optional<glm::dvec3> position = ParseVectorProperty(*origin); position.has_value())
                    playerStart.eyePosition = ConvertMapPosition(*position + glm::dvec3(0.0, 0.0, EyeHeightAboveOrigin));
            }

            if (const std::string* angle = FindProperty(*entity, "angle"); angle != nullptr)
            {
                double degrees = 0.0;
                const char* end = angle->data() + angle->size();
                if (std::from_chars(angle->data(), end, degrees).ec == std::errc())
                    playerStart.yaw = ConvertMapAngleToYaw(degrees);
            }

            return playerStart;
        }
    }

    LoadedLevel SpawnLevel(entt::registry& registry, Renderer::RenderAssets& assets, const MapData& map,
                           const std::string& mapName)
    {
        LoadedLevel level;
        level.playerStart = ReadPlayerStart(map);

        const MapEntity* world = FindEntity(map, "worldspawn");
        if (world == nullptr)
        {
            Core::Log::Write(LogCategory::World, LogLevel::Error, "The map {} has no worldspawn entity", mapName);

            return level;
        }

        LevelMesh levelMesh = BuildLevelMesh(*world);
        level.statistics = levelMesh.statistics;

        // The level has no textures yet (they come in the next branch), so it is drawn by the solid shaded program,
        // which does not read a texture: the texture handle stays empty.
        level.geometry = registry.create();
        registry.emplace<Core::Name>(level.geometry, "World geometry");
        registry.emplace<Core::Transform>(level.geometry);
        registry.emplace<Renderer::MeshRenderer>(level.geometry, Renderer::MeshRenderer{
            .mesh = assets.meshes.Add(mapName, levelMesh.data),
            .shaderProgram = assets.shaders.Load("Shaders/SolidShaded"),
        });

        Core::Log::Write(LogCategory::World, LogLevel::Info, "Level {} loaded: {} brushes, {} faces, {} triangles", mapName,
                         level.statistics.brushCount, level.statistics.faceCount, level.statistics.triangleCount);

        return level;
    }
}
