# Level Editing

Levels of Abomination are built in **[TrenchBroom](https://trenchbroom.github.io/)**,
the level editor for Quake-like games, and saved as `.map` files in
`Assets/Maps/`. The game reads these files directly.

## Setting up TrenchBroom (once per machine)

TrenchBroom supports many games; it has to be told about ours: where our
textures are and which entities (player start, later enemies, pickups, doors)
can be placed. This is described by two files kept in the repository:

| File | Content |
|------|---------|
| `Tools/TrenchBroom/Abomination/GameConfig.cfg` | Name, map format (Valve 220), where textures are |
| `Tools/TrenchBroom/Abomination/Abomination.fgd` | Entity definitions: what can be placed, size and color in the editor |

1. **Download** the latest TrenchBroom for Windows from its
   [releases](https://github.com/TrenchBroom/TrenchBroom/releases) and unpack
   it to a permanent folder (it needs no installation).
2. **Connect the game configuration.** TrenchBroom looks for custom games in
   `%APPDATA%\TrenchBroom\games`. Instead of copying our folder there, create a
   link to it, so changes in the repository are seen by TrenchBroom at once.
   In a command prompt (`cmd`) from the root of the repository:
   ```
   mkdir "%APPDATA%\TrenchBroom\games"
   mklink /J "%APPDATA%\TrenchBroom\games\Abomination" "%CD%\Tools\TrenchBroom\Abomination"
   ```
   (`mklink /J` creates a directory junction: a folder that shows the contents
   of another folder. It needs no administrator rights.)
3. **Set the game path.** Start TrenchBroom, open *Preferences → Games*,
   select *Abomination* and set *Game Path* to the root of the repository
   (the folder with `Assets/`).

## Units and axes

- **32 units = 1 meter.** The Quake player is 56 units tall (about 1.75 m);
  the grid sizes of TrenchBroom (8, 16, 32, 64) are based on this scale.
- **Z is up** in TrenchBroom. The game converts to its own axes (Y up) when it
  loads a map.

## Building the test map

`Assets/Maps/Test.map` is the first map, used to develop map loading:

1. *File → New Map*, choose *Abomination*, map format *Valve*.
2. In the material browser (the *Face* tab of the right panel) select `Crate`
   in the *Textures* collection: new brushes get the selected material. A map
   stores the texture of a face by its file name without the extension
   (`Crate` for `Assets/Textures/Crate.png`).
3. **Room.** In the 3D view, left-drag on the grid to draw a box of about
   512 × 512 units, then raise it to 256 units (hold `Alt` while dragging to
   change the height). With the box selected, use *Edit → CSG → Hollow*: the
   box becomes six brushes, walls, floor and ceiling, with the empty room
   inside.
4. **Pillar.** Inside the room draw a box of 64 × 64 units from the floor to
   the ceiling.
5. **Stairs.** In the tool options of the shape tool choose *Stairs* and draw
   a staircase of 4–6 steps against a wall.
6. **Player start.** In the entity browser (right panel) drag
   `info_player_start` onto the floor.
7. Save as `Assets/Maps/Test.map`.

## Seeing the map in the game

The game loads `Assets/Maps/Test.map` at startup: save the map in
TrenchBroom, build (so the changed file is copied next to the executable)
and start the game. The camera appears at `info_player_start`, looking in the
direction of its angle.

- Until brushes get textures, every face is drawn in one gray, lighter or
  darker by the direction it faces, so walls, floor and ceiling are told
  apart.
- *View > Renderer* in the debug overlay (<kbd>F1</kbd>) switches to
  *Wireframe* to show how faces are split into triangles, and shows how many
  brushes, faces and triangles the level has.
- A map that cannot be read stops the game with an error dialog giving the
  line of the problem. A brush face that does not make sense (three points on
  one line) is skipped.
- Avoid faces of different objects lying in one plane (a box standing flush
  with the top of a step shows flickering stripes there, *z-fighting*).
