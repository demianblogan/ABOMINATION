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
| spdlog  | 1.17.0  | MIT     | Logging (`Core/Log`), built without fmt  | https://github.com/gabime/spdlog |

## Development only

Used for building or testing; not part of the shipped game.

| Tool / library | Version | License      | Purpose                         | Website                                   |
|----------------|---------|--------------|---------------------------------|-------------------------------------------|
| GoogleTest     | 1.18.0  | BSD-3-Clause | Unit tests (`AbominationTests`) | https://github.com/google/googletest      |
| vcpkg          | —       | MIT          | C++ package manager             | https://github.com/microsoft/vcpkg        |
| CMake          | —       | BSD-3-Clause | Build system generator          | https://cmake.org                         |
