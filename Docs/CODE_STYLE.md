# Code Style

Rules for all C++ and GLSL code in this repository. Formatting is enforced by
[`.clang-format`](../.clang-format); everything else is enforced by review.
New rules are added to the end of the relevant section when a deviation is
found during review.

## 1. Language and tooling

- **C++23**, MSVC, warning level **`/W4`**, warnings are treated as errors.
- Source files are **UTF-8 without BOM** (`/utf-8` compiler flag).
- Code, comments, identifiers, log messages and docs are in **English**.
- Format with clang-format before committing
  (Visual Studio: *Ctrl+K, Ctrl+D*, or format on save).

## 2. Files and folders

| Item                 | Rule                                  | Example                              |
|----------------------|---------------------------------------|--------------------------------------|
| Header               | `.hpp`                                | `ShaderProgram.hpp`                  |
| Source               | `.cpp`                                | `ShaderProgram.cpp`                  |
| Code file name       | PascalCase, named after its main type | `FlyCamera.hpp`                      |
| Folder (any)         | PascalCase                            | `Source/Renderer/`, `Assets/Textures/` |
| GLSL                 | PascalCase + stage extension          | `TexturedMesh.vert`, `.frag`         |
| Asset and data file  | PascalCase                            | `Weapons.json`, `RocketLauncher.glb` |
| Tests                | `<TestedFile>Tests.cpp`               | `FlyCameraTests.cpp`                 |

- One main type per header. Small helper types that belong to it may live in
  the same file.
- Headers use `#pragma once`.

## 3. Naming

| Entity                              | Style                  | Example                            |
|-------------------------------------|------------------------|------------------------------------|
| Namespace                           | PascalCase             | `Abomination::Renderer`            |
| Class, struct, enum, alias, concept | PascalCase             | `ShaderProgram`, `EntityID`        |
| Template parameter                  | PascalCase             | `template <typename Component>`    |
| Function, method (any)              | PascalCase             | `LoadTexture()`, `Update()`        |
| Local variable, parameter           | camelCase              | `deltaTime`, `vertexCount`         |
| Private/protected data member       | `m_` + camelCase       | `m_programID`, `m_health`          |
| Public field of a plain struct      | camelCase, no prefix   | `position`, `health`               |
| Constant (incl. `constexpr`)        | PascalCase, no prefix  | `MaxPointLights`                   |
| Enum class value                    | PascalCase             | `WindowMode::Borderless`           |

- **Abbreviations are always written in capitals**, in every kind of name,
  including local variables: `GLBuffer`, `LoadJSON()`, `HUDLayer`,
  `m_entityID`, `playerHUD`, `JSONText`, `ID`.
- Booleans read as a question: `isGrounded`, `hasArmor`, `canJump`.
- Functions start with a verb: `CreateWindow()`, `ApplyDamage()`.
  Simple getters have no `Get` prefix and are named after the member:
  member `m_width` → getter `Width()`. Setters use `Set`: `SetVSync(bool)`.
- Macros are avoided. A naming rule for them will be added if one is ever
  needed.
- No Hungarian notation (`iCount`, `pData`, `strName`).
- The root namespace is `Abomination`. Every module has a nested namespace
  matching its folder: `Source/Renderer/` → `Abomination::Renderer`.

## 4. Formatting

Handled by clang-format. The main choices:

- 4 spaces, no tabs. Line limit **100** columns.
- **Allman** braces — every brace on its own line.
- Contents of a namespace are **indented**.
- Pointer and reference bind to the type: `int* ptr`, `const Mesh& mesh`.
- Always use braces for `if`/`for`/`while` bodies, even for one line.

```cpp
namespace Abomination::Renderer
{
    class ShaderProgram
    {
    public:
        explicit ShaderProgram(std::uint32_t programID) noexcept;

        [[nodiscard]] std::uint32_t ProgramID() const noexcept { return m_programID; }

        void Bind() const
        {
            if (m_programID == 0)
            {
                return;
            }
            glUseProgram(m_programID);
        }

    private:
        std::uint32_t m_programID = 0;
    };
} // namespace Abomination::Renderer
```

## 5. Includes

Order, separated by a blank line (clang-format sorts inside groups):

1. The matching header (in a `.cpp`).
2. Project headers — quotes, path from `Source/`: `#include "Renderer/Mesh.hpp"`.
3. Third-party headers — angle brackets: `#include <glm/glm.hpp>`.
4. Standard library headers: `#include <vector>`.

- Include what you use; do not rely on transitive includes.
- Prefer forward declarations in headers when a full type is not needed.

## 6. Modern C++ usage

**Ownership and resources**

- No raw `new`/`delete`. Use values, `std::unique_ptr`, rarely
  `std::shared_ptr` (only for truly shared ownership).
- Raw pointers and references are **non-owning** only.
- Every resource (OpenGL objects, SDL handles, files) is wrapped in an **RAII**
  type. OpenGL wrappers are **move-only**: copy is deleted, move transfers the
  ID and zeroes the source.

**Functions and classes**

- `const` everything that can be `const`.
- `[[nodiscard]]` on functions whose result must not be ignored.
- `noexcept` on move operations, destructors and trivial getters.
- `override` on overridden virtual functions; `final` when inheritance is not
  intended. Constructors with one argument are `explicit`.
- Default member initializers instead of initializing in constructors when
  possible: `float m_speed = 5.0f;`.
- `auto` when the type is obvious from the right side or is noise
  (iterators, lambdas); spell the type when it carries meaning.
- `enum class` only, never plain `enum`.
- Fixed-width integers (`std::uint32_t`, `std::int16_t`) for data with a
  defined size (file formats, GPU data); `int`/`std::size_t` otherwise.
- `std::string_view` / `std::span` for non-owning parameters.

**Errors**

- **Programmer errors** (broken invariants) → assertions (active in Debug).
- **Recoverable errors** (missing file, bad JSON, shader compile error) →
  return `std::expected<T, Error>`.
- **Fatal startup errors** (no OpenGL 4.6) → log and exit with a message box.
- Exceptions are not thrown by our code. Exceptions from third-party libraries
  (e.g. nlohmann-json) are caught at the boundary where the library is used.

**OpenGL**

- OpenGL types and calls (`GLuint`, `gl*`) appear only inside the `Renderer`
  module. The rest of the code talks to the renderer's high-level API
  (see `ARCHITECTURE.md`). There is no graphics-API abstraction layer
  (RHI): OpenGL is the only backend.
- Use **Direct State Access** (OpenGL 4.5+): `glCreateBuffers`,
  `glNamedBufferStorage`, `glTextureStorage2D`, … — not bind-to-edit.

## 7. Comments

- Comments explain **why**, not what. Good names explain what.
- Public API in headers gets a short `///` comment when its behaviour is not
  obvious from the name.
- `// TODO(Module): ...` for known unfinished work; do not leave commented-out
  code.

## 8. Units and coordinate system

- Distances in **meters**, time in **seconds**, angles in **radians**
  (convert degrees only at the boundary: UI, config files).
- World space is **right-handed, Y-up**, −Z is forward — the OpenGL/glm
  convention. Data from other conventions (TrenchBroom is Z-up) is converted
  once, at load time.

## 9. GLSL

- `#version 460 core` at the top of every shader.
- Functions PascalCase, locals and parameters camelCase — same as C++.
- Shader-global variables are PascalCase; a prefix shows where they come
  from:

| Kind                                                      | Style              | Example                                 |
|-----------------------------------------------------------|--------------------|-----------------------------------------|
| Vertex attribute — `in` of the vertex shader (from a VBO) | `a` + PascalCase   | `aPosition`, `aTexCoord`                |
| Uniform (including samplers)                              | `uni` + PascalCase | `uniViewProjection`, `uniAlbedoTexture` |
| `in`/`out` between stages — same name on both sides       | PascalCase         | `TexCoord`, `WorldNormal`               |
| Output of the fragment shader (to the framebuffer)        | PascalCase         | `FragColor`                             |

- Explicit `layout(location = N)` / `layout(binding = N)` on all inputs,
  outputs, samplers and blocks.

```glsl
// TexturedMesh.vert
#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

layout(location = 0) uniform mat4 uniViewProjection;

layout(location = 0) out vec2 TexCoord;

void main()
{
    TexCoord = aTexCoord;
    gl_Position = uniViewProjection * vec4(aPosition, 1.0);
}
```

```glsl
// TexturedMesh.frag
#version 460 core

layout(location = 0) in vec2 TexCoord;

layout(binding = 0) uniform sampler2D uniAlbedoTexture;

layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = texture(uniAlbedoTexture, TexCoord);
}
```

## 10. Data files (JSON)

- Keys are **snake_case**: `"move_speed": 7.5`.
- Units follow section 8 unless the key says otherwise (`"fov_degrees": 90`).

## 11. Tests

- GoogleTest. One test file per tested unit: `FlyCameraTests.cpp`.
- `TEST(Suite, Behavior)`: suite is the tested type, behavior describes the
  expected result in PascalCase: `TEST(FlyCamera, MovesForwardAlongViewDirection)`.
- Arrange / Act / Assert, separated by blank lines.
- Tests do not need an OpenGL context. Code that needs one is kept thin so
  the logic around it can be tested separately.
