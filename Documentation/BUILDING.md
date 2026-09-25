# Building

How to get the source, build the game and run the tests on Windows.

## Requirements

| Tool                 | Version        | Notes                                                        |
|----------------------|----------------|--------------------------------------------------------------|
| Windows              | 10 or 11, x64  | The only supported platform                                  |
| Visual Studio        | 2026           | Workload **Desktop development with C++** (includes CMake)   |
| Git                  | any recent     | Also used by vcpkg to fetch library recipes                  |
| vcpkg                | any recent     | Library versions are pinned by `builtin-baseline` in `vcpkg.json` |
| GPU driver           | OpenGL 4.6     | Required once rendering is added (milestone 0.1)             |

## 1. Install vcpkg (once per machine)

```bash
git clone https://github.com/microsoft/vcpkg.git C:/Development/vcpkg
C:/Development/vcpkg/bootstrap-vcpkg.bat -disableMetrics
```

Set the environment variable **`VCPKG_ROOT`** to the vcpkg folder
(*Settings → System → About → Advanced system settings → Environment
Variables*), then restart Visual Studio and terminals. `CMakePresets.json`
finds vcpkg through this variable.

## 2. Get the source

```bash
git clone https://github.com/demianblogan/Abomination.git
```

## 3. Build in Visual Studio (recommended)

1. **File → Open → Folder…** and select the repository root.
2. Visual Studio detects `CMakePresets.json` and configures the project.
   The first configuration downloads and builds all libraries through
   vcpkg — this takes a few minutes once, later runs are fast.
3. Select the configure preset **Windows (Visual Studio 2026, x64)** and the
   build preset **Debug** or **Release** in the toolbar.
4. **Build → Build All**, then **F5** runs the game.
5. **Test → Test Explorer → Run All** runs the unit tests.

## 4. Build from the command line

Use the **same CMake that Visual Studio uses** — the one bundled with it:

```
C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\
```

If no other CMake is installed, it is available as plain `cmake` in the
**Developer PowerShell for VS 2026**. A separately installed CMake comes first
on `PATH` even there — then call the bundled `cmake.exe` / `ctest.exe` by
their full path.

```bash
cmake --preset windows-msvc
cmake --build --preset debug
ctest --preset debug
```

Replace `debug` with `release` for an optimized build.

> [!WARNING]
> Do not mix different CMake versions on the same build folder (for example a
> separately installed CMake and the one inside Visual Studio). Files generated
> by a newer CMake may not be readable by an older CTest, and Test Explorer
> will then show no tests. If this happens, run
> *Project → Delete Cache and Reconfigure* in Visual Studio.

## 5. Build the game package

The package is the folder a player receives: everything needed to run the game
on another computer, without Visual Studio or the Visual C++ Redistributable.

```bash
cmake --build --preset release
cmake --install Build/windows-msvc --config Release
```

The folder `Build/Package/` then contains `Abomination.exe`, `Assets/`, the
Microsoft C++ runtime DLLs, `LICENSE.md` and `Licenses/` with the licenses of
the libraries. What goes into it is described in `CMake/Packaging.cmake`.
Delete `Build/Package/` before installing again: files removed from the
project are not removed from an existing package.

## Output

Everything is generated in `Build/<preset>/` (ignored by Git):

| Path                                   | Content                       |
|----------------------------------------|-------------------------------|
| `Build/windows-msvc/Abomination.slnx`  | Generated Visual Studio solution |
| `Build/windows-msvc/Binaries/Debug/`   | `Abomination.exe`, `AbominationTests.exe` |
| `Build/windows-msvc/Binaries/Release/` | Same, optimized               |
| `Build/Package/`                       | The game package (after `cmake --install`) |

The generated solution can be opened directly, but project settings must be
changed only in `CMakeLists.txt` — edits made in Visual Studio's project
properties are lost on the next CMake run.
