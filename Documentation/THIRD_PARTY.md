# Third-Party Software

Every library the project uses is listed here with its license. A library is
added to this list in the same pull request that adds it to `vcpkg.json`.
This list is the source for the libraries section of the in-game Credits
screen. Third-party **assets** are listed separately in
[ASSETS.md](ASSETS.md).

## Distributed with the game

Libraries linked into `Abomination.exe`. Their licenses must be respected in
every release.

| Library | Version | License | Purpose                                  | Website                          |
|---------|---------|---------|------------------------------------------|----------------------------------|
| GLAD    | 2.0.8   | (WTFPL OR CC0-1.0) AND Apache-2.0 | OpenGL 4.6 Core loader, generated into `ThirdParty/GLAD` | https://gen.glad.sh |
| glm     | 1.0.3   | MIT     | Math: vectors, matrices (header-only)    | https://github.com/g-truc/glm    |
| SDL3    | 3.4.16  | Zlib    | Window, OpenGL context, events, input    | https://www.libsdl.org           |
| spdlog  | 1.17.0  | MIT     | Logging (`Core/Log`), built without fmt  | https://github.com/gabime/spdlog |

## Development only

Used for building or testing; not part of the shipped game.

| Tool / library | Version | License      | Purpose                         | Website                                   |
|----------------|---------|--------------|---------------------------------|-------------------------------------------|
| GoogleTest     | 1.18.0  | BSD-3-Clause | Unit tests (`AbominationTests`) | https://github.com/google/googletest      |
| vcpkg          | —       | MIT          | C++ package manager             | https://github.com/microsoft/vcpkg        |
| CMake          | —       | BSD-3-Clause | Build system generator          | https://cmake.org                         |
