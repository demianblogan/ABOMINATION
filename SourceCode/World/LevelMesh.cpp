#include "World/LevelMesh.h"

#include "World/BrushGeometry.h"
#include "World/MapCoordinates.h"

#include <glm/geometric.hpp>

#include <cstddef>
#include <cstdint>

namespace Abomination::World
{
    LevelMesh BuildLevelMesh(const MapEntity& entity)
    {
        LevelMesh result;
        Renderer::MeshData& data = result.data;
        LevelMeshStatistics& counts = result.statistics;

        for (const MapBrush& brush : entity.brushes)
        {
            ++counts.brushCount;

            for (const Core::ConvexPolygon& polygon : BuildBrushPolygons(brush))
            {
                // A face that does not exist (a plane that misses the brush) has no corners.
                if (polygon.empty())
                    continue;

                ++counts.faceCount;

                // The corners of the face in game coordinates. They are counter-clockwise seen from the front, so the
                // cross product of two edges points out of the face: that is its normal.
                const auto firstVertex = static_cast<std::uint32_t>(data.vertices.size());
                for (const glm::dvec3& corner : polygon)
                    data.vertices.push_back(Renderer::MeshVertex{.position = ConvertMapPosition(corner)});

                const glm::vec3 edge1 = data.vertices[firstVertex + 1].position - data.vertices[firstVertex].position;
                const glm::vec3 edge2 = data.vertices[firstVertex + 2].position - data.vertices[firstVertex].position;
                const glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
                for (std::size_t index = firstVertex; index < data.vertices.size(); ++index)
                    data.vertices[index].normal = normal;

                // A convex polygon is cut into triangles like a fan: corner 0 with every pair of neighbours after it,
                // (0, 1, 2), (0, 2, 3), (0, 3, 4), ... A polygon with N corners gives N - 2 triangles, all
                // counter-clockwise like the polygon itself.
                const auto cornerCount = static_cast<std::uint32_t>(polygon.size());
                for (std::uint32_t corner = 1; corner + 1 < cornerCount; ++corner)
                {
                    data.indices.insert(data.indices.end(), {firstVertex, firstVertex + corner, firstVertex + corner + 1});
                    ++counts.triangleCount;
                }
            }
        }

        return result;
    }
}
