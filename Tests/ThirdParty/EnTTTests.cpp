#include <entt/entt.hpp>
#include <gtest/gtest.h>

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
}
