#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace Abomination::Renderer
{
    // A block of memory owned by the graphics driver, usually in video memory: vertices, indices, shader data.
    // The data is uploaded once when the buffer is created and cannot be changed afterwards ("immutable storage"),
    // which lets the driver place it in the fastest memory. The buffer is deleted in the destructor. Move-only.
    class GLBuffer
    {
    public:
        // Creates the buffer and copies the bytes into it. std::as_bytes(std::span(array)) turns any array into bytes.
        explicit GLBuffer(std::span<const std::byte> data);

        GLBuffer(const GLBuffer&) = delete;
        GLBuffer& operator=(const GLBuffer&) = delete;

        GLBuffer(GLBuffer&& other) noexcept;
        GLBuffer& operator=(GLBuffer&& other) noexcept;

        ~GLBuffer();

        // The name OpenGL gave the buffer; other OpenGL wrappers (GLVertexArray) need it.
        [[nodiscard]] std::uint32_t GetID() const noexcept;

    private:
        // 0 means "no buffer" (a moved-from object).
        std::uint32_t m_bufferID = 0;
    };
}
