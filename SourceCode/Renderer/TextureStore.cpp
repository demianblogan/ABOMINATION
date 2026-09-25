#include "Renderer/TextureStore.h"

#include "Core/Image.h"
#include "Core/Log.h"

#include <cstddef>
#include <cstdint>
#include <expected>
#include <optional>
#include <string>
#include <utility>

namespace Abomination::Renderer
{
    using Core::LogCategory;
    using Core::LogLevel;

    namespace
    {
        // The checkerboard is 8x8 texels, one texel per square. With the crisp GL_NEAREST filtering of our textures,
        // a face showing it has 64 sharp squares, whatever the size of the face.
        constexpr int FallbackTextureSize = 8;

        // Creates the magenta and black checkerboard in memory.
        GLTexture CreateFallbackTexture()
        {
            Core::Image image;
            image.width = FallbackTextureSize;
            image.height = FallbackTextureSize;
            constexpr int ByteCount = FallbackTextureSize * FallbackTextureSize * Core::ImageChannelCount;
            image.pixels.reserve(static_cast<std::size_t>(ByteCount));

            for (int y = 0; y < FallbackTextureSize; ++y)
            {
                for (int x = 0; x < FallbackTextureSize; ++x)
                {
                    // (x + y) is even and odd in turn along every row and every column: a checkerboard.
                    const bool isMagentaSquare = (x + y) % 2 == 0;
                    const std::uint8_t redAndBlue = isMagentaSquare ? 255 : 0;
                    image.pixels.insert(image.pixels.end(), {redAndBlue, 0, redAndBlue, 255});
                }
            }

            return GLTexture::CreateFromImage(image);
        }
    }

    TextureStore::TextureStore(std::filesystem::path assetsDirectory)
        : m_assetsDirectory(std::move(assetsDirectory))
        , m_fallbackTexture(CreateFallbackTexture())
    {}

    TextureHandle TextureStore::Load(const std::string& path)
    {
        if (const std::optional<TextureHandle> loadedHandle = m_cache.Find(path); loadedHandle.has_value())
            return *loadedHandle;

        // make_preferred() turns the forward slashes of the asset path into the backslashes of Windows,
        // so paths in log messages do not mix both.
        std::filesystem::path fullPath = m_assetsDirectory / path;
        fullPath.make_preferred();

        std::expected<GLTexture, std::string> texture = GLTexture::CreateFromFile(fullPath);
        if (!texture.has_value())
        {
            Core::Log::Write(LogCategory::Renderer, LogLevel::Warning, "Texture {} replaced by the fallback: {}", path,
                             texture.error());

            return m_cache.Add(path, CreateFallbackTexture());
        }

        Core::Log::Write(LogCategory::Renderer, LogLevel::Debug, "Texture loaded: {}", path);

        return m_cache.Add(path, std::move(*texture));
    }

    const GLTexture& TextureStore::Get(TextureHandle handle) const
    {
        const GLTexture* texture = m_cache.Get(handle);
        if (texture == nullptr)
            return m_fallbackTexture;

        return *texture;
    }
}
