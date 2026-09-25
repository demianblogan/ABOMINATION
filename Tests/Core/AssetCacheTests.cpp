#include "Core/AssetCache.h"

#include <gtest/gtest.h>

#include <memory>
#include <optional>
#include <string>

namespace Abomination::Core
{
    // Strings stand in for real assets: the cache does not care what it stores.
    class AssetCacheTest : public ::testing::Test
    {
    protected:
        AssetCache<std::string> m_cache;
    };

    TEST_F(AssetCacheTest, AddedAssetCanBeReadByHandle)
    {
        const AssetHandle<std::string> handle = m_cache.Add("Textures/Crate.png", "crate");

        ASSERT_NE(m_cache.Get(handle), nullptr);
        EXPECT_EQ(*m_cache.Get(handle), "crate");
        EXPECT_EQ(m_cache.GetCount(), 1u);
    }

    TEST_F(AssetCacheTest, FindReturnsHandleOfLoadedPath)
    {
        const AssetHandle<std::string> handle = m_cache.Add("Textures/Crate.png", "crate");

        const std::optional<AssetHandle<std::string>> foundHandle = m_cache.Find("Textures/Crate.png");

        ASSERT_TRUE(foundHandle.has_value());
        EXPECT_EQ(*foundHandle, handle);
        EXPECT_FALSE(m_cache.Find("Textures/Missing.png").has_value());
    }

    TEST_F(AssetCacheTest, DefaultHandleIsInvalid)
    {
        m_cache.Add("Textures/Crate.png", "crate");

        const AssetHandle<std::string> handle;

        EXPECT_FALSE(m_cache.IsValid(handle));
        EXPECT_EQ(m_cache.Get(handle), nullptr);
    }

    TEST_F(AssetCacheTest, RemovedAssetMakesHandleInvalid)
    {
        const AssetHandle<std::string> handle = m_cache.Add("Textures/Crate.png", "crate");

        m_cache.Remove(handle);

        EXPECT_FALSE(m_cache.IsValid(handle));
        EXPECT_EQ(m_cache.Get(handle), nullptr);
        EXPECT_FALSE(m_cache.Find("Textures/Crate.png").has_value());
        EXPECT_EQ(m_cache.GetCount(), 0u);
    }

    TEST_F(AssetCacheTest, ReusedSlotDoesNotRespondToOldHandle)
    {
        const AssetHandle<std::string> oldHandle = m_cache.Add("Textures/Crate.png", "crate");
        m_cache.Remove(oldHandle);

        const AssetHandle<std::string> newHandle = m_cache.Add("Textures/Wall.png", "wall");

        // The free slot is reused, but with a new generation: the old handle does not give the new asset.
        EXPECT_EQ(newHandle.index, oldHandle.index);
        EXPECT_NE(newHandle.generation, oldHandle.generation);
        EXPECT_EQ(m_cache.Get(oldHandle), nullptr);
        EXPECT_EQ(*m_cache.Get(newHandle), "wall");
    }

    TEST_F(AssetCacheTest, AddingSamePathReplacesAssetAndKeepsHandle)
    {
        const AssetHandle<std::string> handle = m_cache.Add("Textures/Crate.png", "old crate");

        const AssetHandle<std::string> sameHandle = m_cache.Add("Textures/Crate.png", "new crate");

        EXPECT_EQ(sameHandle, handle);
        EXPECT_EQ(*m_cache.Get(handle), "new crate");
        EXPECT_EQ(m_cache.GetCount(), 1u);
    }

    TEST_F(AssetCacheTest, RemovingTwiceDoesNothing)
    {
        const AssetHandle<std::string> handle = m_cache.Add("Textures/Crate.png", "crate");
        const AssetHandle<std::string> otherHandle = m_cache.Add("Textures/Wall.png", "wall");

        m_cache.Remove(handle);
        m_cache.Remove(handle);

        EXPECT_EQ(m_cache.GetCount(), 1u);
        EXPECT_EQ(*m_cache.Get(otherHandle), "wall");
    }

    TEST_F(AssetCacheTest, VisitAssetsSkipsRemovedAssets)
    {
        m_cache.Add("Textures/Crate.png", "crate");
        const AssetHandle<std::string> wallHandle = m_cache.Add("Textures/Wall.png", "wall");
        m_cache.Add("Textures/Door.png", "door");
        m_cache.Remove(wallHandle);

        std::string visited;
        m_cache.VisitAssets([&](const std::string& path, const std::string& asset)
        {
            visited += path + "=" + asset + ";";
        });

        EXPECT_EQ(visited, "Textures/Crate.png=crate;Textures/Door.png=door;");
    }

    TEST(AssetCache, StoresMoveOnlyAssets)
    {
        // GPU resources (GLTexture, GLShaderProgram) can be moved but not copied; the cache must accept them.
        AssetCache<std::unique_ptr<int>> cache;

        const AssetHandle<std::unique_ptr<int>> handle = cache.Add("Numbers/FortyTwo", std::make_unique<int>(42));

        ASSERT_NE(cache.Get(handle), nullptr);
        EXPECT_EQ(**cache.Get(handle), 42);
    }
}
