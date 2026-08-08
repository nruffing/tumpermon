# Tumpermon

A Game Boy homebrew project built with GBDK-2020, started from the `template-minimal`
GBDK example. Built with `lcc`/SDCC, run in Emulicious for dev, flashed to real
cartridges via GBxCart RW + FlashGBX for hardware testing. See `README.md` for
build/run/flash instructions, including the "Adding a New Metasprite" workflow.

**Keep this file up to date.** When a change alters architecture, conventions, or
known gaps described below (not just a one-off bugfix), update the relevant section
in the same pass — don't leave this file describing a design that's since changed.
If unsure whether something rises to that bar, err toward updating it.

## Build

- `make` (macOS/Linux) or `compile.bat` (Windows) from the repo root.
- `GBDK_HOME` defaults to `../../` — this repo is expected to live two directories
  deep inside a GBDK install (e.g. `<gbdk-root>/git/tumpermon`). Override with
  `make GBDK_HOME=/path/to/gbdk` if placed elsewhere.
- Sources are globbed from `src/*.c` and `src/*/*.c` (one subfolder level deep).
- **Sprite/tile art (`res/*.png`) is auto-converted** by `png2asset` into
  `obj/res/<name>.c`/`.h` as part of `make` — no per-asset Makefile changes needed.
  `obj/` is gitignored. See README's "Adding a New Metasprite" for the full workflow
  and PNG requirements (indexed color, tile-aligned dimensions).
- `-Wm-yc` is set in the Makefile to mark the ROM CGB-compatible (required for
  `_cpu == CGB_TYPE` to ever be true at runtime — without it every build silently
  falls back to DMG grayscale, which cost real debugging time early on).
- `make clean` removes build artifacts (also gitignored) and `obj/`.

## Architecture

Each module is layered so it only knows about the layer directly below it:

```
main.c (process_frame)
  → joypad.c        raw joypad() → JoypadState (no knowledge of Player)
  → kinematics.c     JoypadState → Velocity  (compute_velocity_from_joypad)
  → player.c         Velocity → Player state (apply_velocity), then → sprite (update_player_sprite)
```

- **`kinematics.{c,h}`** — generic, reusable motion primitives: `Position`,
  `Velocity`, `Direction`, fixed-point conversion, `move_sprite_to_position`
  (single hardware sprite), `move_metasprite_to_position` (multi-sprite
  metasprite). Deliberately knows nothing about `Player` — intended to be
  reused for any future entity (enemies, projectiles, etc). Both `move_*`
  functions are entity-agnostic: they take a `Metasprite`/sprite number and
  `Position` and know nothing about which entity owns them, so they can't
  accidentally assume they're the only entity in the game (see
  `move_metasprite_to_position`'s `hide_sprites_range` note below).
- **`joypad.{c,h}`** — input layer only. `JoypadState`/`InputState` describe raw
  button state (`is_pressed`, `is_just_pressed` — edge-detected via a `static`
  previous-frame value inside `process_joypad`, shared correctly across all
  callers including the splash screen). Deliberately has no `Player` dependency.
- **`player.{c,h}`** — the only module that knows about `Player`. `apply_velocity`
  mutates position/direction only (no hardware calls); `update_player_sprite` is
  a separate step that pushes state to the sprite (via
  `move_metasprite_to_position`). Kept separate on purpose — when collision
  detection is added, it belongs *between* these two calls (resolve collision
  against the candidate position before committing state, then sync the sprite
  once) rather than mutate-then-revert.
- **`sprites.{h}`** — VRAM tile index / OAM slot constants (`PLAYER_SPRITE_SLOT`,
  `PLAYER_SPRITE_TILE_START_INDEX`, `PLAYER_SPRITE_MAX_COUNT`), kept separate
  from `.c` files so slot/tile assignments across entities are easy to see as
  more sprites are added. Also defines the `Metasprite` struct — a pointer to
  a generated `metasprite_t` frame plus the bookkeeping needed to draw and
  clean up after it (see "Metasprites" below).
- **`colors.h`** — named CGB RGB555 palette constants (`RGB()`/`RGB8()` from
  `gb/cgb.h`).
- **`splash.c`** — title screen, drawn in APA (`gb/drawing.h`) mode. Leaves APA
  mode (`mode(M_TEXT_OUT)`) and explicitly re-blanks VRAM tile pattern data
  (`utils/util.c`'s `reset_screen`) before gameplay starts — `cls()` alone only
  resets the tile *map*, not the tile pattern data APA drew into, and leaving
  that stale data caused visible corruption (vertical bars) after the splash.
- **`utils/`** — `util.c` (CGB detection, background/sprite palette load+reset,
  VRAM/screen reset), `apa_util.c` (APA mode enter/exit, centered-text helpers).

## Metasprites

The player is rendered as a GBDK metasprite (multiple hardware sprites drawn as
one image), not a single hardware sprite — currently a 16x16 image made of four
8x8 tiles, generated from `res/player.png` via `png2asset` (see README). Key
pieces:

- **`res/*.png` → `obj/res/*.c`/`.h`**: generated per-asset symbols
  (`<name>_tiles`, `<name>_palettes`, `<name>_metasprites[]`, `<name>_TILE_COUNT`,
  etc). Regenerated on every `make`; not committed.
- **`Metasprite` struct** (`sprites.h`): bundles a pointer to one generated
  `metasprite_t` frame with the VRAM tile start index, OAM sprite slot, base
  property flags, and `max_sprite_count` — the size of that entity's *reserved*
  OAM slot range (not just what the current frame happens to use). Each entity
  needs its own non-overlapping slot range; see `PLAYER_SPRITE_SLOT`/
  `PLAYER_SPRITE_MAX_COUNT` as the pattern to follow for a new entity.
- **`move_metasprite_to_position`** (`kinematics.c`) positions the metasprite via
  `move_metasprite_ex`, then calls `hide_sprites_range` scoped to
  `sprite_num .. sprite_num + max_sprite_count` — this hides any OAM slots the
  entity reserved but didn't use this frame (e.g. a smaller animation frame
  than a previous one) without ever being able to reach into another entity's
  slots. This only matters once animation/variable sprite counts exist; today
  it's a no-op since the player always uses exactly 4 sprites.
- **Sprite colors need their own palette call.** `set_bkg_palette` (background)
  and `set_sprite_data`/tile loading do **not** implicitly set sprite colors —
  without an explicit `set_sprite_palette` call (wrapped as
  `update_sprite_pallete` in `utils/util.c`, mirroring `update_color_pallete`
  for the background), sprites render with the default/uninitialized palette
  (faded/grayscale). DMG fallback uses `OBP0_REG`, same pattern as `BGP_REG`
  for the background.
- **`move_sprite_to_position`** (single hardware sprite) still exists and is
  currently unused by `Player`, but intentionally kept for future single-tile
  entities (e.g. a simple projectile) that don't need the metasprite machinery.

## Conventions / decisions

- **Fixed-point position/velocity**, not floats (no FPU on this CPU).
  `FIXED_POINT_POSITION_LENGTH = 4` (16 units/pixel). Position is `uint16_t`
  (unsigned, screen-space), Velocity is `int16_t` (signed, for direction).
  Convert to real pixels only at render time (`fixed_point_to_pixel`).
- **Screen/velocity coordinate convention**: +x = right, +y = **down** (not up —
  standard top-left-origin screen space, opposite of math/graph convention).
- **`move_sprite()`/`move_metasprite_ex()` hardware offset**: GB sprite
  coordinates are offset by `+8` (x) / `+16` (y) from the visible screen
  position (hardware reserves margin for sprites scrolling on/off-screen).
  Handled once in `move_sprite_to_position`/`move_metasprite_to_position`, not
  something callers need to think about.
- **Opposing D-pad directions cancel to 0** rather than one arbitrarily winning
  (`compute_velocity_from_joypad` checks e.g. `left && !right`). Note: this
  couldn't actually be exercised/tested — Emulicious (like real hardware) never
  reports both Left+Right pressed simultaneously, so the cancellation logic is
  correct but currently unverified by observed behavior.
- **Struct composition**: value-embed sub-structs (`Player.position` is a
  `Position`, not a `Position *`) — no heap/malloc on this platform, so a
  pointer member would just add indirection with no ownership benefit. This
  does *not* apply to references into shared/const ROM data (e.g.
  `Metasprite.metasprite` is a `const metasprite_t *` into a generated,
  immutable frame array) — those are pointers by necessity (GBDK's API takes a
  pointer; there's no fixed-size value to copy) and by design (shared,
  read-only data, not owned state). Functions still take pointers (`Player *`)
  when they need to mutate.
- **`static` for file-local helpers** (e.g. `fixed_point_to_pixel`,
  `build_input_state`) — not exposed in the header, since they're
  implementation details of their `.c` file.
- Prefer designated initializers for structs: `Player p = { .position = ... };`.
- `(void)` explicitly on zero-arg function signatures (C, not C++, semantics —
  empty `()` means "unspecified args," not "no args").

## Known gaps / next steps

- **Player has real art but only one frame** — `res/player.png` (16x16, 2x2
  tiles) replaced the old placeholder solid tile, loaded via the metasprite
  pipeline (see "Metasprites" above), but there's still only one static frame.
  No walk cycle, no direction-facing sprite swap yet.
- **`JoypadState` doesn't cover A/B/Select yet** — only up/down/left/right/start.
- **No collision detection** — `apply_velocity` applies velocity unconditionally.
  Planned approach (see Architecture above): candidate position → resolve
  against world (per-axis, not combined, to allow wall-sliding) → commit →
  sync sprite. Not yet implemented.
- **No animation** — direction-facing state exists on `Player`, but nothing
  currently changes sprite tiles/frames based on it. When animation frames are
  added, `move_metasprite_flipx`/`_flipy` (hardware sprite flip) are the
  natural fit for direction-facing without doubling up on stored art.
- **Sub-pixel movement is implemented but movement is simple** — constant
  velocity per frame while a direction is held, no acceleration/friction, no
  diagonal-speed normalization (diagonal movement is faster than cardinal).
- **`hide_sprites_range` cleanup in `move_metasprite_to_position` is currently
  inert** — correct and scoped per-entity (see "Metasprites" above), but has
  nothing to do until sprite counts actually vary frame-to-frame (animation,
  despawning, or variable entity counts).

## SDCC/GBDK gotchas encountered (useful if debugging build warnings)

- **Angle brackets (`<foo.h>`) vs quotes (`"foo.h"`)**: project-local headers
  need quotes — angle brackets only search GBDK's system include paths, not
  `src/`. Same applies to generated asset headers (`<res/player.h>`) — those
  need angle brackets since they're reached via `-Iobj` (the include path
  points at `obj/`, so `<res/player.h>` resolves to `obj/res/player.h`;
  pointing `-I` at `obj/res` itself instead is the wrong level and causes a
  "file not found" on `<res/...>` includes).
- **`warning 158: overflow in implicit constant conversion`**: SDCC sometimes
  doesn't constant-fold `#define`-based arithmetic (e.g. `GRAPHICS_WIDTH - 1`)
  before checking it against a narrower parameter type, even when the result
  provably fits — a false positive. Fix: explicit cast, e.g.
  `(uint8_t)(GRAPHICS_WIDTH - 1)`.
- **`warning 110`, "so said EVELYN the modified DOG"** (e.g.
  `src/utils/apa_util.c:39`): SDCC's (intentionally quirky) message for its
  optimizer eliminating/rewriting a conditional branch it proved was constant.
  This is known-harmless and already triaged — no need to re-investigate it or
  flag it again when it shows up in build output.
- **`gprint()` takes non-`const char *`** — GBDK's older APA text API isn't
  const-correct; keep your own strings `const` and cast at the `gprint()` call
  site specifically, rather than dropping `const` everywhere.
- **Editor (clangd) diagnostics vs. real build**: VS Code's C/C++ IntelliSense
  frequently can't resolve GBDK's include paths in this project (`gb/gb.h`,
  `gb/metasprites.h`, etc. show as "file not found"), which cascades into
  bogus "undeclared identifier" errors for everything downstream in that file.
  This is editor-config noise, not a real compile error — always confirm with
  an actual `make` before treating a red squiggly as a real problem.
