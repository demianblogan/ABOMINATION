#include <entt/entt.hpp>
#include <gtest/gtest.h>

#include <algorithm>
#include <vector>

// Checks that EnTT compiles with the project settings (C++23, /permissive-, /W4 /WX) and behaves as the game expects.
// The game code itself is tested in the Gameplay and Renderer tests once it uses EnTT.
namespace Abomination
{
    namespace
    {
        struct Position
        {
            float x = 0.0f;
        };

        struct Velocity
        {
            float x = 0.0f;
        };
    }

    TEST(EnTT, ViewVisitsOnlyEntitiesWithAllComponents)
    {
        entt::registry registry;

        const entt::entity moving = registry.create();
        registry.emplace<Position>(moving, 1.0f);
        registry.emplace<Velocity>(moving, 2.0f);

        const entt::entity standing = registry.create();
        registry.emplace<Position>(standing, 5.0f);

        // Only the entity with both components moves.
        for (auto [entity, position, velocity] : registry.view<Position, Velocity>().each())
            position.x += velocity.x;

        EXPECT_FLOAT_EQ(registry.get<Position>(moving).x, 3.0f);
        EXPECT_FLOAT_EQ(registry.get<Position>(standing).x, 5.0f);
    }

    TEST(EnTT, DestroyedEntityIsNoLongerValid)
    {
        entt::registry registry;
        const entt::entity entity = registry.create();

        registry.destroy(entity);

        // Like our AssetHandle: the entity number holds a version, so a destroyed entity is recognized.
        EXPECT_FALSE(registry.valid(entity));
    }

    TEST(EnTT, EntityViewVisitsAllLivingEntities)
    {
        entt::registry registry;
        const entt::entity first = registry.create();
        const entt::entity destroyed = registry.create();
        const entt::entity last = registry.create();
        registry.destroy(destroyed);

        // view<entt::entity>() visits every entity that exists, whatever components it has (the entity inspector
        // lists entities this way); destroyed ones are skipped.
        std::vector<entt::entity> visited;
        for (const entt::entity entity : registry.view<entt::entity>())
            visited.push_back(entity);

        EXPECT_EQ(visited.size(), 2u);
        EXPECT_NE(std::ranges::find(visited, first), visited.end());
        EXPECT_NE(std::ranges::find(visited, last), visited.end());
    }
}
