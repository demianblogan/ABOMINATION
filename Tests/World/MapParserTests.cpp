#include "World/MapParser.h"

#include <gtest/gtest.h>

#include <string>
#include <string_view>

namespace Abomination::World
{
    namespace
    {
        // A small map in the format TrenchBroom writes: the world with one brush of two faces (a real brush has at
        // least four, but the parser does not check geometry), and a player start. The second face continues on the next
        // line: tokens are separated by any spaces or line breaks.
        constexpr std::string_view SmallMap = R"(// Game: Abomination
// Format: Valve
// entity 0
{
"mapversion" "220"
"classname" "worldspawn"
// brush 0
{
( 48 64 240 ) ( 48 -448 240 ) ( 48 64 -16 ) Crate [ 0 1 0 0 ] [ 0 0 -1 0 ] 270 1 1
( -301.25483399593907 -256 128 ) ( -320 -274.74516600406093 64 ) ( 0 0 0 )
Wall [ -0.7071 -0.7071 0 -7.76 ] [ 0 0 -1 8 ] 0 0.5 2
}
}
// entity 1
{
"classname" "info_player_start"
"origin" "-48 -176 88"
"angle" "180"
}
)";
    }

    TEST(MapParser, ReadsEntitiesAndProperties)
    {
        const std::expected<MapData, std::string> map = ParseMap(SmallMap);

        ASSERT_TRUE(map.has_value()) << map.error();
        ASSERT_EQ(map->entities.size(), 2u);

        const MapEntity& world = map->entities[0];
        ASSERT_NE(FindProperty(world, "classname"), nullptr);
        EXPECT_EQ(*FindProperty(world, "classname"), "worldspawn");
        EXPECT_EQ(*FindProperty(world, "mapversion"), "220");

        const MapEntity& playerStart = map->entities[1];
        EXPECT_EQ(*FindProperty(playerStart, "classname"), "info_player_start");
        EXPECT_EQ(*FindProperty(playerStart, "origin"), "-48 -176 88");
        EXPECT_TRUE(playerStart.brushes.empty());
        EXPECT_EQ(FindProperty(playerStart, "target"), nullptr);
    }

    TEST(MapParser, ReadsFacesOfBrushes)
    {
        const std::expected<MapData, std::string> map = ParseMap(SmallMap);
        ASSERT_TRUE(map.has_value()) << map.error();
        ASSERT_EQ(map->entities[0].brushes.size(), 1u);

        const MapBrush& brush = map->entities[0].brushes[0];
        ASSERT_EQ(brush.faces.size(), 2u);

        const MapFace& first = brush.faces[0];
        EXPECT_EQ(first.points[0], glm::dvec3(48.0, 64.0, 240.0));
        EXPECT_EQ(first.points[1], glm::dvec3(48.0, -448.0, 240.0));
        EXPECT_EQ(first.points[2], glm::dvec3(48.0, 64.0, -16.0));
        EXPECT_EQ(first.textureName, "Crate");
        EXPECT_EQ(first.textureUAxis, glm::dvec3(0.0, 1.0, 0.0));
        EXPECT_EQ(first.textureVAxis, glm::dvec3(0.0, 0.0, -1.0));
        EXPECT_DOUBLE_EQ(first.textureRotation, 270.0);
    }

    TEST(MapParser, KeepsFullPrecisionOfNumbers)
    {
        const std::expected<MapData, std::string> map = ParseMap(SmallMap);
        ASSERT_TRUE(map.has_value()) << map.error();

        const MapFace& second = map->entities[0].brushes[0].faces[1];
        EXPECT_DOUBLE_EQ(second.points[0].x, -301.25483399593907);
        EXPECT_DOUBLE_EQ(second.points[1].y, -274.74516600406093);
        EXPECT_EQ(second.textureName, "Wall");
        EXPECT_DOUBLE_EQ(second.textureOffsetU, -7.76);
        EXPECT_DOUBLE_EQ(second.textureOffsetV, 8.0);
        EXPECT_DOUBLE_EQ(second.textureScaleU, 0.5);
        EXPECT_DOUBLE_EQ(second.textureScaleV, 2.0);
    }

    TEST(MapParser, EmptyTextGivesEmptyMap)
    {
        const std::expected<MapData, std::string> map = ParseMap("// only a comment\n");

        ASSERT_TRUE(map.has_value());
        EXPECT_TRUE(map->entities.empty());
    }

    TEST(MapParser, ReportsLineOfMistake)
    {
        // Line 3: the first point is missing its closing bracket.
        const std::expected<MapData, std::string> map =
            ParseMap("{\n{\n( 0 0 0  ( 1 0 0 ) ( 0 1 0 ) Crate [ 1 0 0 0 ] [ 0 1 0 0 ] 0 1 1\n}\n}\n");

        ASSERT_FALSE(map.has_value());
        EXPECT_EQ(map.error(), "Line 3: expected ')' but found '('");
    }

    TEST(MapParser, ReportsUnclosedEntity)
    {
        const std::expected<MapData, std::string> map = ParseMap("{\n\"classname\" \"worldspawn\"\n");

        ASSERT_FALSE(map.has_value());
        EXPECT_EQ(map.error(), "Line 3: the entity is not closed with '}'");
    }

    TEST(MapParser, RejectsOldQuakeFormat)
    {
        // The original Quake format writes offsets, rotation and scale as plain numbers, without texture axes.
        const std::expected<MapData, std::string> map =
            ParseMap("{\n{\n( 0 0 0 ) ( 1 0 0 ) ( 0 1 0 ) Crate 0 0 0 1 1\n}\n}\n");

        ASSERT_FALSE(map.has_value());
        EXPECT_EQ(map.error(), "Line 3: expected '[' (the map must be saved in the Valve 220 format) but found '0'");
    }

    TEST(MapParser, ReportsBrokenNumber)
    {
        const std::expected<MapData, std::string> map =
            ParseMap("{\n{\n( 0 0 zero ) ( 1 0 0 ) ( 0 1 0 ) Crate [ 1 0 0 0 ] [ 0 1 0 0 ] 0 1 1\n}\n}\n");

        ASSERT_FALSE(map.has_value());
        EXPECT_EQ(map.error(), "Line 3: expected a number but found 'zero'");
    }
}
