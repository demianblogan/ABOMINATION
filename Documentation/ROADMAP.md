# Roadmap

A living draft. It is reviewed after every milestone release and may change
at any time. Its main purpose is to answer two questions quickly: *where are
we now* and *what comes next*.

## Principles

- **Every milestone adds something to play and something to see.** Constant
  gameplay and visual feedback keeps development motivating.
- **Learning OpenGL comes first.** Rendering features are introduced one
  concept at a time and explained in depth.
- **Architecture grows with need.** No speculative systems (YAGNI), but
  refactoring is done as soon as a need appears.
- **Versions:** `0.1` … `0.9`, then `1.0` release — see
  [GIT_CONVENTIONS.md](GIT_CONVENTIONS.md#7-versions-and-releases).

**Status legend:** ✅ done · 🔨 in progress · ⏳ planned

## Overview

| Version | Name                   | Status | Gameplay                                              | Visual / OpenGL                                          |
|---------|------------------------|--------|-------------------------------------------------------|----------------------------------------------------------|
| 0.1     | Foundation             | 🔨     | Free-fly (noclip) camera                              | Window, OpenGL 4.6 context, debug output, textured cube, ImGui overlay |
| 0.2     | First Steps            | ⏳     | Quake-style movement, collision with the level        | EnTT, resource manager, TrenchBroom map loading, brush texturing |
| 0.3     | Boomstick              | ⏳     | First hitscan weapon, damage, sound                   | View model with sway/bob/recoil, muzzle flash, particles, decals, glTF loading |
| 0.4     | It Moves               | ⏳     | First enemy: AI, navmesh, health, death, HUD          | Skeletal animation, text rendering                       |
| 0.5     | Lights                 | ⏳     | Glowing projectiles, dynamic light in combat          | Lightmap baking, shadows, HDR, bloom, gamma              |
| 0.6     | Game Loop              | ⏳     | Pickups, armor, doors, buttons, plates, level exit, stats screen, level transitions — first complete level | Moving brushes, data-driven configs |
| 0.7     | Arsenal & Bestiary     | ⏳     | All weapons, projectiles, explosions, weapon switching (keys, wheel, mouse wheel), gamepad, new enemy types (incl. flying), first boss | Weapon effects, new models |
| 0.8     | Menus & Saves          | ⏳     | Autosave, manual save, quicksave, load menu           | Animated main menu, pause menu, all options, key rebinding, 5 languages, logo screen, language selection |
| 0.9     | Content Complete (Beta)| ⏳     | All 4 episodes, bosses, story texts, achievements, DualSense features | Episode palettes, polish, optimization |
| 1.0     | Release                | ⏳     | Balance, bug fixes                                    | —                                                        |

## 0.1 — Foundation 🔨

**Goal:** a professional project skeleton and the first 3D image on screen
that you can fly around.

| # | Branch                          | Status | Content                                                                 |
|---|---------------------------------|--------|-------------------------------------------------------------------------|
| 1 | `docs/project-foundation`       | ✅     | Roadmap, code style, git conventions, architecture draft, assets list, PR template, clang-format, editorconfig |
| 2 | `build/cmake-vcpkg-setup`       | ✅     | CMake, presets, vcpkg manifest, folder structure, empty executable, GoogleTest, CI on GitHub Actions |
| 3 | `feat/logging`                  | ✅     | spdlog logging: levels, per-module categories, console and log file     |
| 4 | `feat/window-gl-context`        | ✅     | SDL3 window, OpenGL 4.6 Core context, GLAD 2 loader, debug output, frame timer, animated clear color |
| 5 | `feat/debug-overlay`            | ✅     | Dear ImGui overlay: version, GPU, FPS, frame time graph; keyboard state (`Input` module), F1 toggle |
| 6 | `feat/textured-cube`            | ⏳     | Assets folder copied next to the executable; shaders, buffers, vertex array, texture — all with DSA and RAII wrappers; JetBrains Mono font for the debug overlay |
| 7 | `feat/fly-camera`               | ⏳     | Mouse state, input actions and bindings (F1 becomes `ToggleDebugOverlay`), camera, mouse + WASD flight, first math unit tests |

**Done when:** the game opens a window, shows a textured cube that can be
examined with a free-fly camera, the debug overlay shows FPS, CI builds and
tests every PR.

## Later milestones

Detailed branch plans are written when a milestone starts. Notes collected so
far:

- **0.2** — decide collision approach: own AABB-vs-brush collision with a BVH;
  Jolt Physics only for queries if it becomes necessary. Learn TrenchBroom:
  game configuration, entity definitions, `.map` format, Z-up → Y-up.
- **0.3** — asset pipeline decision: where models come from (generated,
  downloaded, bought) and how they are imported. Audio via miniaudio.
- **0.4** — navmesh via Recast/Detour for ground enemies; flying enemies need
  a separate approach.
- **0.5** — own lightmap compiler as part of the level compiler.
- **0.8** — every gameplay component must be serializable; keep this in mind
  from 0.2 onwards.

## Backlog

Ideas that are not assigned to a milestone yet.

- **Developer tools window** (ImGui): buttons and checkboxes instead of typing —
  list of levels to load, god mode, fly through walls, give weapons. Useful once
  levels and gameplay exist (around 0.6).
- **Developer console** in the style of Half-Life: readable long command names
  (`load_level`, `toggle_god_mode`) with autocompletion while typing and
  history. Only if the tools window is not enough.
  Both would call the same **command registry**, so a button and a typed
  command run the same code.
- **In-game log viewer**: a third log sink keeping recent messages in memory,
  shown in the debug overlay with filters by level and category (useful in
  Release builds without a console window).
