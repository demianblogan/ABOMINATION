#include "World/LevelMesh.h"
#include "World/MapParser.h"

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <gtest/gtest.h>

#include <cmath>
#include <cstddef>
#include <string>
#include <string_view>

namespace Abomination::World
{
    namespace
    {
        constexpr float Tolerance = 1e-5f;

        // A cube of 64 units (2 meters) with one corner at the map origin, and a player start.
        constexpr std::string_view CubeMap = R"({
"classname" "worldspawn"
{
( 0 0 64 ) ( 0 0 0 ) ( 0 64 0 ) Crate [ 0 1 0 0 ] [ 0 0 -1 0 ] 0 1 1
( 64 64 0 ) ( 64 0 0 ) ( 64 0 64 ) Crate [ 0 1 0 0 ] [ 0 0 -1 0 ] 0 1 1
( 64 0 0 ) ( 0 0 0 ) ( 0 0 64 ) Crate [ 1 0 0 0 ] [ 0 0 -1 0 ] 0 1 1
( 0 64 64 ) ( 0 64 0 ) ( 64 64 0 ) Crate [ 1 0 0 0 ] [ 0 0 -1 0 ] 0 1 1
( 0 64 0 ) ( 0 0 0 ) ( 64 0 0 ) Crate [ 1 0 0 0 ] [ 0 -1 0 0 ] 0 1 1
( 64 0 64 ) ( 0 0 64 ) ( 0 64 64 ) Crate [ 1 0 0 0 ] [ 0 -1 0 0 ] 0 1 1
}
}
)";

        LevelMesh BuildFromMap(std::string_view mapText)
        {
            const std::expected<MapData, std::string> map = ParseMap(mapText);
            EXPECT_TRUE(map.has_value()) << (map.has_value() ? "" : map.error());

            return BuildLevelMesh(map.value().entities.at(0));
        }
    }

    TEST(LevelMesh, CubeGivesTwoTrianglesPerFace)
    {
        const LevelMesh mesh = BuildFromMap(CubeMap);

        EXPECT_EQ(mesh.statistics.brushCount, 1);
        EXPECT_EQ(mesh.statistics.faceCount, 6);
        EXPECT_EQ(mesh.statistics.triangleCount, 12);
        EXPECT_EQ(mesh.data.vertices.size(), 24u);
        EXPECT_EQ(mesh.data.indices.size(), 36u);
    }

    TEST(LevelMesh, VerticesAreInGameMetersAndAxes)
    {
        const LevelMesh mesh = BuildFromMap(CubeMap);
        ASSERT_FALSE(mesh.data.vertices.empty());

        // Map x and y from 0 to 64 and z from 0 to 64 become game x from 0 to 2, y from 0 to 2 and z from -2 to 0.
        for (const Renderer::MeshVertex& vertex : mesh.data.vertices)
        {
            EXPECT_TRUE(std::abs(vertex.position.x) < Tolerance || std::abs(vertex.position.x - 2.0f) < Tolerance);
            EXPECT_TRUE(std::abs(vertex.position.y) < Tolerance || std::abs(vertex.position.y - 2.0f) < Tolerance);
            EXPECT_TRUE(std::abs(vertex.position.z) < Tolerance || std::abs(vertex.position.z + 2.0f) < Tolerance);
        }
    }

    TEST(LevelMesh, NormalsPointOutOfCube)
    {
        const LevelMesh mesh = BuildFromMap(CubeMap);
        ASSERT_FALSE(mesh.data.vertices.empty());
        const glm::vec3 cubeCenter(1.0f, 1.0f, -1.0f);

        for (const Renderer::MeshVertex& vertex : mesh.data.vertices)
        {
            EXPECT_NEAR(glm::length(vertex.normal), 1.0f, Tolerance);
            EXPECT_GT(glm::dot(vertex.normal, vertex.position - cubeCenter), 0.0f);
        }
    }

    TEST(LevelMesh, TrianglesGoCounterClockwiseSeenFromOutside)
    {
        // The front side of every triangle (counter-clockwise order) must face the same way as the normal of its face.
        const LevelMesh mesh = BuildFromMap(CubeMap);
        ASSERT_FALSE(mesh.data.indices.empty());

        for (std::size_t first = 0; first < mesh.data.indices.size(); first += 3)
        {
            const Renderer::MeshVertex& a = mesh.data.vertices[mesh.data.indices[first]];
            const Renderer::MeshVertex& b = mesh.data.vertices[mesh.data.indices[first + 1]];
            const Renderer::MeshVertex& c = mesh.data.vertices[mesh.data.indices[first + 2]];

            const glm::vec3 triangleFacing = glm::cross(b.position - a.position, c.position - a.position);
            EXPECT_GT(glm::dot(triangleFacing, a.normal), 0.0f);
        }
    }
}
