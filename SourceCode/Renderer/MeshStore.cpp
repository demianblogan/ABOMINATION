#include "Renderer/MeshStore.h"

#include "Core/Log.h"
#include "Renderer/MeshPrimitives.h"

namespace Abomination::Renderer
{
    using Core::LogCategory;
    using Core::LogLevel;

    MeshStore::MeshStore()
        : m_fallbackMesh(Mesh::Create(CreateCubeMeshData()))
    {}

    MeshHandle MeshStore::Add(const std::string& name, const MeshData& data)
    {
        // An empty buffer cannot be created in OpenGL (glNamedBufferStorage needs a size above 0), and there would be
        // nothing to draw anyway.
        if (data.vertices.empty() || data.indices.size() < 3)
        {
            Core::Log::Write(LogCategory::Renderer, LogLevel::Warning, "Mesh {} has no triangles, replaced by the fallback",
                             name);

            return m_cache.Add(name, Mesh::Create(CreateCubeMeshData()));
        }

        Core::Log::Write(LogCategory::Renderer, LogLevel::Debug, "Mesh added: {} ({} vertices, {} indices)", name,
                         data.vertices.size(), data.indices.size());

        return m_cache.Add(name, Mesh::Create(data));
    }

    std::optional<MeshHandle> MeshStore::Find(const std::string& name) const
    {
        return m_cache.Find(name);
    }

    const Mesh& MeshStore::Get(MeshHandle handle) const
    {
        const Mesh* mesh = m_cache.Get(handle);
        if (mesh == nullptr)
            return m_fallbackMesh;

        return *mesh;
    }

    std::size_t MeshStore::GetCount() const noexcept
    {
        return m_cache.GetCount();
    }
}
