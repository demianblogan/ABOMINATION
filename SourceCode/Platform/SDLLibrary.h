#pragma once

#include <expected>
#include <string>

namespace Abomination::Platform
{
    // Owns the initialized SDL library: SDL is started in Initialize() and shut down in the destructor.
    // Only one object may exist at a time. Move-only, so the ownership can be passed on but never duplicated.
    class SDLLibrary
    {
    public:
        [[nodiscard]] static std::expected<SDLLibrary, std::string> Initialize();

        SDLLibrary(const SDLLibrary&) = delete;
        SDLLibrary& operator=(const SDLLibrary&) = delete;

        SDLLibrary(SDLLibrary&& other) noexcept;
        SDLLibrary& operator=(SDLLibrary&& other) noexcept;

        ~SDLLibrary();

    private:
        SDLLibrary() noexcept = default;

        // False for an object whose ownership was moved to another object: such an object must not shut SDL down.
        bool m_isActive = false;
    };
}
