<div align="center">

<!-- Logo goes here once it is ready:
<img src="Documentation/Images/Logo.png" alt="Abomination" width="480">
-->

# ABOMINATION

**A retro first-person shooter inspired by Quake (1996),<br>
built from scratch with modern C++ and OpenGL 4.6.**

[![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp/23)
[![OpenGL 4.6](https://img.shields.io/badge/OpenGL-4.6_Core-5586A4?style=for-the-badge&logo=opengl&logoColor=white)](https://www.khronos.org/opengl/)
[![SDL3](https://img.shields.io/badge/SDL-3-1D4E89?style=for-the-badge)](https://www.libsdl.org/)
![Windows](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)
<br>
[![CI](https://img.shields.io/github/actions/workflow/status/demianblogan/Abomination/CI.yml?branch=main&style=flat-square&label=CI&logo=githubactions&logoColor=white)](https://github.com/demianblogan/Abomination/actions/workflows/CI.yml)
[![Status](https://img.shields.io/badge/status-early_development-orange?style=flat-square)](Documentation/ROADMAP.md)
[![Milestone](https://img.shields.io/badge/milestone-0.1_Foundation-blue?style=flat-square)](Documentation/ROADMAP.md)
[![License](https://img.shields.io/badge/license-PolyForm_Noncommercial_1.0-lightgrey?style=flat-square)](LICENSE.md)

[About](#-about) •
[Features](#-features) •
[Tech Stack](#%EF%B8%8F-tech-stack) •
[Roadmap](#%EF%B8%8F-roadmap) •
[Building](#-building) •
[Documentation](#-documentation) •
[License](#-license)

</div>

---

## 🩸 About

**Abomination** is a fast-paced, old-school shooter: no reloading, no
cutscenes, no hand-holding — just you, a growing arsenal and whatever crawls,
flies and teleports out of the dark.

The project is also a deep dive into graphics programming: the engine is
written from the ground up, and every system — from the OpenGL renderer to
the enemy AI — is built to be read and learned from.

> [!NOTE]
> The game is in early development. Nothing is playable yet —
> follow the [roadmap](Documentation/ROADMAP.md) to see what is being built right now.

## 🎯 Features

<table>
<tr>
<td width="50%" valign="top">

### 🕹️ Gameplay
- **4 episodes × 5 levels**, a unique boss at the end of each episode
- **Quake-style movement** — fast, fluid, skill-based
- **Varied arsenal** — every weapon has its own model and feel
- **Diverse enemies** — they run, crawl, jump, fly, climb walls and teleport
- Doors, buttons, pressure plates, traps and secrets

</td>
<td width="50%" valign="top">

### 🎨 Presentation
- **Retro look** — low-poly models, low-resolution pixel-crisp textures
- **Modern lighting** — baked lightmaps, dynamic lights, HDR, bloom
- Unique color palette for every episode
- Full gamepad support, including **DualSense** haptics, adaptive triggers
  and light bar
- 5 languages: English, Español, Deutsch, Русский, Українська

</td>
</tr>
</table>

## 🛠️ Tech Stack

| Area            | Technology                                                                 |
|-----------------|----------------------------------------------------------------------------|
| Language        | C++23 (MSVC, `/W4 /WX`)                                                    |
| Graphics        | OpenGL 4.6 Core · Direct State Access · GLSL 4.60 · GLAD 2                 |
| Platform        | SDL3 — window, input, gamepads                                             |
| Architecture    | ECS with EnTT                                                              |
| Math            | glm                                                                        |
| Logging         | spdlog                                                                     |
| Levels          | TrenchBroom + own level compiler                                           |
| Build           | CMake · vcpkg · GitHub Actions                                             |
| Testing         | GoogleTest · CTest                                                         |

## 🔨 Building

Requires **Visual Studio 2026** (Desktop development with C++) and
**[vcpkg](https://github.com/microsoft/vcpkg)** with `VCPKG_ROOT` set.

```bash
git clone https://github.com/demianblogan/Abomination.git
```

Open the folder in Visual Studio — it picks up `CMakePresets.json`, vcpkg
fetches all libraries, and **F5** runs the game.
Full instructions, including the command line: **[BUILDING.md](Documentation/BUILDING.md)**.

## 🗺️ Roadmap

| Version | Milestone              | Status |
|:-------:|------------------------|:------:|
| 0.1     | Foundation             | 🔨     |
| 0.2     | First Steps            | ⏳     |
| 0.3     | Boomstick              | ⏳     |
| 0.4     | It Moves               | ⏳     |
| 0.5     | Lights                 | ⏳     |
| 0.6     | Game Loop              | ⏳     |
| 0.7     | Arsenal & Bestiary     | ⏳     |
| 0.8     | Menus & Saves          | ⏳     |
| 0.9     | Content Complete       | ⏳     |
| **1.0** | **Release**            | ⏳     |

<sub>✅ done · 🔨 in progress · ⏳ planned — details in [ROADMAP.md](Documentation/ROADMAP.md)</sub>

## 📚 Documentation

| Document                                   | What's inside                                         |
|--------------------------------------------|-------------------------------------------------------|
| 🗺️ [Roadmap](Documentation/ROADMAP.md)               | Milestones from 0.1 to 1.0 and the current plan       |
| 🔨 [Building](Documentation/BUILDING.md)           | Requirements and build instructions                   |
| 🏛️ [Architecture](Documentation/ARCHITECTURE.md)     | Modules, dependency rules, main loop, renderer, ECS   |
| ✍️ [Code Style](Documentation/CODE_STYLE.md)         | Naming, formatting and C++/GLSL conventions           |
| 🌿 [Git Conventions](Documentation/GIT_CONVENTIONS.md) | Branches, commits, pull requests, versions          |
| 📦 [Assets](Documentation/ASSETS.md)                 | Third-party assets and their licenses                 |
| 🧩 [Third-Party](Documentation/THIRD_PARTY.md)     | Libraries and tools with their licenses               |

## 📜 License

The source code is licensed under the
**[PolyForm Noncommercial License 1.0.0](LICENSE.md)** — you are welcome to
read, study and modify it for any noncommercial purpose.
Third-party assets keep their own licenses, see [ASSETS.md](Documentation/ASSETS.md).

---

<div align="center">

Made with 🩸 by **Alone Bull**

</div>
