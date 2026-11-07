# TEST SUBJECT 001 — Game Boy

An original 2D puzzle-platformer for the original Nintendo Game Boy, built in C with GBDK-2020.

## Build a real .gb ROM

This repository builds the ROM automatically with GitHub Actions.

1. Open the **Actions** tab.
2. Select **Build Game Boy ROM**.
3. Open the latest successful run.
4. Under **Artifacts**, download `TEST_SUBJECT_001-Game-Boy-ROM`.
5. Unzip it to get `TEST_SUBJECT_001.gb`.

A push to `main` also starts a build automatically. No GB Studio installation is needed.

## Current prototype controls

- D-pad: move.
- A: pick up or drop the cube.
- B: use the linked portal.
- SELECT: switch between the FRONT and BACK test-room layers.
- START: begin the test.

## Design plan

The intended game is a substantial, original Game Boy tribute to first-person portal puzzle games, redesigned around a 2D side-view / room-layer mechanic:

- Two linked portal endpoints and portal-based room traversal.
- Test chambers with weighted cubes, floor switches, doors, moving platforms, hazards and timed puzzles.
- Layer-shift chambers that make the room appear to turn in 3D; portals connect the two visible sides.
- Puzzle elements that can be repositioned, cut out of one chamber area and placed into another as part of the test logic.
- A sarcastic laboratory AI announcer, delivered through text rather than voice acting.
- Original chiptune music and sound effects.
- A multi-phase final AI boss encounter.

## Development status

This is the first code-and-build milestone, not the finished feature list. It currently contains a simple text-rendered movement/puzzle prototype and a basic two-side room concept. Portal physics, polished tile/sprite artwork, full puzzle progression, audio, and the final boss are planned follow-up milestones. All graphics and music will be original Game Boy-style assets, not ripped Portal game files.

## Toolchain

- GBDK-2020 4.5.0
- GitHub Actions
- Target: original Game Boy / DMG-compatible ROM
