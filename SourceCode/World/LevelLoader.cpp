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
        // The texture of a face is a file in Assets/Textures, named in the map by its path there without the extension:
        // "Episode1/Wall_MossyBrick" -> "Textures/Episode1/Wall_MossyBrick.png".
        std::string MakeTexturePath(const std::string& textureName)
        {
            return "Textures/" + textureName + ".png";
        }

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

        // Texture coordinates need the size of every texture, so the textures of the level are loaded here. A missing
        // texture gets the checkerboard fallback of the store (and a warning in the log), with the size of the fallback.
        const TextureSizeLookup getTextureSize = [&assets](const std::string& textureName)
        {
            const Renderer::GLTexture& texture = assets.textures.Get(assets.textures.Load(MakeTexturePath(textureName)));
            return glm::ivec2(texture.GetWidth(), texture.GetHeight());
        };
        LevelMesh levelMesh = BuildLevelMesh(*world, getTextureSize);
        level.statistics = levelMesh.statistics;

        // Until the level is drawn grouped by texture (the next step), it is drawn by the solid shaded program, which does
        // not read a texture: the texture handle stays empty.
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
