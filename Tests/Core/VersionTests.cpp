#include "Core/Version.h"

#include <gtest/gtest.h>

#include <format>
#include <string>

namespace Abomination::Core
{
    TEST(Version, StringMatchesComponents)
    {
        const Version version = GameVersion();

        const std::string expected =
            std::format("{}.{}.{}", version.major, version.minor, version.patch);

        EXPECT_EQ(GameVersionString(), expected);
    }

    TEST(Version, EqualWhenAllComponentsAreEqual)
    {
        const Version first{.major = 1, .minor = 2, .patch = 3};
        const Version same{.major = 1, .minor = 2, .patch = 3};
        const Version otherPatch{.major = 1, .minor = 2, .patch = 4};

        EXPECT_EQ(first, same);
        EXPECT_NE(first, otherPatch);
    }
} // namespace Abomination::Core
