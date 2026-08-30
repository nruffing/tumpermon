# Tumpermon

A Game Boy homebrew project built with GBDK-2020, started from the `template-minimal`
GBDK example. Built with `lcc`/SDCC, run in Emulicious for dev, flashed to real
cartridges via GBxCart RW + FlashGBX for hardware testing. See `README.md` for
build/run/flash instructions, including the "Adding a New Metasprite" workflow.

Formatting follows `.clang-format` at the repo root, auto-applied to staged
`.c`/`.h` files by a `pre-commit` hook (`.githooks/pre-commit`) — enabled per-clone
via `git config core.hooksPath .githooks` (see README Setup step 6). Don't
hand-format against a different style; let the hook (or VS Code format-on-save)
handle it.

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
- `make force` runs `clean` then `all` — a full rebuild, for when the
  incremental build is suspect.
- `make romsize` rebuilds with the linker's `-Wl-m` map-file flag and prints
  actual ROM bytes used out of the 32KB budget (plain file size on the built
  `.gb` always reads 100% with no MBC — the ROM is padded to a fixed size
  regardless of how much is actually used, so it's not useful for this).

## Architecture

Each module is layered so it only knows about the layer directly below it:

```
main.c (process_frame)
  → joypad.c        raw joypad() → JoypadState (no knowledge of Player)
  → kinematics.c     JoypadState → Velocity  (compute_velocity_from_joypad)
  → player.c         Velocity → Player state (apply_player_velocity), then → sprite (update_player_sprite)
```

- **`kinematics.{c,h}`** — generic, reusable motion primitives: `Position`,
  `Velocity`, `Direction`, fixed-point conversion, `move_sprite_to_position`
  (single hardware sprite), `move_metasprite_to_position` (multi-sprite
  metasprite, one fixed frame), `move_animated_metasprite_to_position`
  (multi-sprite metasprite, frame picked by `Direction`). Deliberately knows
  nothing about `Player` — intended to be reused for any future entity
  (enemies, projectiles, etc). All three `move_*` functions are
  entity-agnostic: they take a `Metasprite`/`AnimatedMetasprite`/sprite
  number and `Position` (plus `Direction` for the animated one) and know
  nothing about which entity owns them, so they can't accidentally assume
  they're the only entity in the game (see `move_metasprite_to_position`'s
  `hide_sprites_range` note below). Both metasprite variants funnel through
  a shared `static` helper, `move_metasprite_ref`, that does the actual
  `move_metasprite_ex`/`move_metasprite_flipx` call (picking flip based on
  `MetaspriteRef.flip_x`) and the `hide_sprites_range` cleanup — added so
  direction-facing's mirroring didn't need duplicating between the two
  callers.
- **`joypad.{c,h}`** — input layer only. `JoypadState`/`InputState` describe raw
  button state (`is_pressed`, `is_just_pressed` — edge-detected via a `static`
  previous-frame value inside `process_joypad`, shared correctly across all
  callers including the splash screen). Deliberately has no `Player` dependency.
- **`context.h`** — `Context`, the per-frame state bundle threaded through
  `main.c`'s loop: `tick`, `is_paused`, a value-embedded `JoypadState
  joypad_state` (value, not pointer — see "Struct composition" below), a
  value-embedded `Enemy enemies[MAX_ENEMIES]` (same reasoning — see "Enemies"
  below), and pointers to `Player`/`KinematicBehaviorContext`. `process_frame`
  populates `joypad_state` and `tick` each frame, then calls `handle_pause`
  (toggles `is_paused` on `Start`) before doing anything movement-related;
  when `is_paused` the rest of `process_frame` (velocity/sprite update) is
  skipped for that frame.
- **`player.{c,h}`** — the only module that knows about `Player`. `apply_player_velocity`
  mutates position/direction only (no hardware calls); `update_player_sprite` is
  a separate step that pushes state to the sprite (via
  `move_animated_metasprite_to_position`, passing `player->direction` so the
  right facing renders). Kept separate on purpose — when collision
  detection is added, it belongs *between* these two calls (resolve collision
  against the candidate position before committing state, then sync the sprite
  once) rather than mutate-then-revert. Also defines `create_player_metasprite`
  (see "Metasprites" below), which builds the player's `AnimatedMetasprite`
  once at startup.
- **`sprites.{h}`** — VRAM tile index / OAM slot constants (`PLAYER_SPRITE_SLOT`,
  `PLAYER_SPRITE_TILE_START_INDEX`, `PLAYER_SPRITE_MAX_COUNT`), kept separate
  from `.c` files so slot/tile assignments across entities are easy to see as
  more sprites are added. Also defines the metasprite type family — see
  "Metasprites" below.
- **`colors.h`** — named CGB RGB555 palette constants (`RGB()`/`RGB8()` from
  `gb/cgb.h`).
- **`splash.c`** — title screen, drawn in APA (`gb/drawing.h`) mode. Leaves APA
  mode (`mode(M_TEXT_OUT)`) and explicitly re-blanks VRAM tile pattern data
  (`utils/util.c`'s `reset_screen`) before gameplay starts — `cls()` alone only
  resets the tile *map*, not the tile pattern data APA drew into, and leaving
  that stale data caused visible corruption (vertical bars) after the splash.
- **`utils/`** — `util.c` (CGB detection, background/sprite palette load+reset,
  VRAM/screen reset), `apa_util.c` (APA mode enter/exit, centered-text helpers),
  `metasprite_util.c` (`pad_metasprite_animation_frames`, see "Metasprites"
  below).

## Metasprites

The player is rendered as a GBDK metasprite (multiple hardware sprites drawn as
one image), not a single hardware sprite — currently a 16x16 image made of four
8x8 tiles, generated from `res/player.png` via `png2asset` (see README). Key
pieces:

- **`res/*.png` → `obj/res/*.c`/`.h`**: generated per-asset symbols
  (`<name>_tiles`, `<name>_palettes`, `<name>_metasprites[]`, `<name>_TILE_COUNT`,
  etc). Regenerated on every `make`; not committed.
- **`res/player.png` is a 48x16 sheet of three 16x16 direction-facing
  frames stacked horizontally, in order: down, up, left/right (mirror for
  the opposite side).** No walk-cycle frames yet — one static frame per
  facing, intentionally. Split into `player_metasprites[0..2]` by passing
  `-sw 16` to `png2asset` — see the explicit `$(RESOBJDIR)/player.c` rule in
  the Makefile (the generic pattern rule doesn't know the frame size, so a
  multi-frame sheet needs its own rule; see README's "Adding a New
  Metasprite" step 2). Do **not** hand-run `png2asset` without an explicit
  `-o`/`-c` output path against a file under `res/` — it defaults to writing
  generated output next to the source PNG, which pollutes `res/` with
  generated `.c`/`.h` that don't belong there (only `obj/res/` should ever
  hold generated output).
- **The metasprite type family** (`sprites.h`) splits into pieces so
  direction-facing and future animation don't duplicate entity-level
  bookkeeping:
  - **`MetaspriteMetadata`** — the entity-level, frame-independent state:
    VRAM tile start index, OAM sprite slot, base property flags, and
    `max_sprite_count` (that entity's *reserved* OAM slot range — see
    `PLAYER_SPRITE_SLOT`/`PLAYER_SPRITE_MAX_COUNT`). Same for every frame/
    direction of a given entity.
  - **`MetaspriteRef`** — one renderable frame: a pointer to a generated
    `metasprite_t` plus `flip_x` (mirror it horizontally via
    `move_metasprite_flipx` instead of storing a separate mirrored frame).
  - **`Metasprite`** — `MetaspriteRef` + `MetaspriteMetadata`. A single
    fixed frame; currently unused by `Player` (which needs direction-facing)
    but kept for a future entity that's multi-tile but doesn't need it —
    e.g. a non-directional decoration or effect.
  - **`MetaspriteAnimationFrames`** — one direction's worth of animation
    frames: `MetaspriteRef refs[METASPRITE_MAX_ANIMATION_FRAMES]`, wrapped
    in its own struct (rather than a bare array) specifically so it can be
    built and assigned by value — see `pad_metasprite_animation_frames`
    below. Convention: index 0 is idle; pad the rest with the idle ref for
    directions that don't have (or don't yet have) distinct walk frames.
  - **`AnimatedMetasprite`** — `MetaspriteAnimationFrames frames[4]`
    (indexed by `Direction`) + one shared `MetaspriteMetadata`. This is
    `Player.metasprite`'s type.
  - **`pad_metasprite_animation_frames(MetaspriteRef idle_ref)`**
    (`utils/metasprite_util.c`) — builds a `MetaspriteAnimationFrames` with
    every slot set to `idle_ref`. Deliberately knows nothing about
    `AnimatedMetasprite` or `Direction`; it only builds one direction's
    frame set, so it composes cleanly regardless of how many directions or
    entities end up using it.
  - **`create_player_metasprite`** (`player.c`) builds the player's
    `AnimatedMetasprite`: one `MetaspriteRef` per direction from
    `player_metasprites[0..2]` (down→0, up→1, right→2 native, left→2
    mirrored via `flip_x`), each padded via `pad_metasprite_animation_frames`
    since there's no walk-cycle art yet (every animation-frame slot repeats
    the single idle-like pose for that direction).
- **`move_metasprite_to_position`** (fixed `Metasprite`) and
  **`move_animated_metasprite_to_position`** (`AnimatedMetasprite` +
  `Direction` — looks up `metasprite.frames[direction].refs[0]`, i.e.
  always the idle/first frame today; see "Known gaps") both funnel through
  `kinematics.c`'s `static move_metasprite_ref` helper, which picks
  `move_metasprite_ex` vs. `move_metasprite_flipx` based on
  `MetaspriteRef.flip_x`, then calls `hide_sprites_range` scoped to
  `sprite_num .. sprite_num + max_sprite_count` — this hides any OAM slots
  the entity reserved but didn't use this frame (e.g. a smaller animation
  frame than a previous one) without ever being able to reach into another
  entity's slots. The `hide_sprites_range` part is currently a no-op since
  the player always uses exactly 4 sprites regardless of direction.
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

## Enemies

Unlike `Player` (a 2x2-tile `AnimatedMetasprite`), `Enemy` is a single 8x8
hardware sprite — the simpler path `kinematics.c`'s `move_sprite_to_position`
was originally kept around for (see "Metasprites" above). `sprites.h` reserves
its VRAM tile index and OAM slot range the same way it does for the player
(`ENEMY_TILE_START_INDEX`, `ENEMY_SPRITE_START_SLOT`), just without the
metasprite metadata machinery since there's only ever one tile/one OAM slot
per enemy.

- **`enemy.{c,h}`** — `Enemy` (`sprite_num`, `Position`). `initialize_enemy`
  assigns it an OAM slot (`ENEMY_SPRITE_START_SLOT + enemy_index`) via
  `set_sprite_tile`. `apply_enemy_velocity`/`update_enemy_sprite` mirror
  `Player`'s `apply_player_velocity`/`update_player_sprite` split (mutate
  position, then separately sync the sprite) but are named per-entity rather
  than shared — C has no function overloading, so `Player` and `Enemy` can't
  both define a plain `apply_velocity`/`update_sprite` without a link
  conflict. This is the established convention for any future entity type:
  `apply_<entity>_velocity`, `update_<entity>_sprite`.
- **Enemies intentionally reuse the player's CGB object palette** rather than
  loading their own. `initialize_enemy` calls
  `set_sprite_prop(sprite_num, OAMF_CGB_PAL0)` instead of ever loading
  `enemy_palettes` (the palette `obj/res/enemy.h` generates from
  `res/enemy.png` is unused dead data) — `OAMF_CGB_PAL0` is the same palette
  slot `update_sprite_pallete(player_palettes)` loads in `main.c`. This means
  **`res/enemy.png`'s pixel *indices*, not its own colors, are what
  determine how an enemy renders** — each pixel's index is looked up against
  `player_palettes` at render time. `res/enemy.png` is authored so its
  indices line up with `player_palettes`' order (0=white/transparent,
  1=light gray, 2=dark gray, 3=black) — its Makefile rule passes
  `-keep_palette_order` specifically so `png2asset` preserves that index
  order instead of resorting the PNG's palette by color frequency (the
  default behavior, and what caused a real bug: the majority-color
  background pixels landed at index 0 — always hardware-transparent for
  sprites regardless of color — making the circle disappear and the
  background render in `player_palettes[1]`, light gray). **If
  `player_palettes`' index order ever changes, `res/enemy.png`'s pixel
  indices need to be re-mapped to match**, and any new single-sprite entity
  following this pattern needs the same `-keep_palette_order` + matching
  index order.
- **`seed_enemies`** (`main.c`) fills a caller-owned `Enemy` array —
  currently spawns every enemy at the same hardcoded placeholder position
  (see "Known gaps" below for what's still missing before this is real).

## Conventions / decisions

- **Fixed-point position/velocity**, not floats (no FPU on this CPU).
  `FIXED_POINT_POSITION_LENGTH = 4` (16 units/pixel). Position is `uint16_t`
  (unsigned, screen-space), Velocity is `int16_t` (signed, for direction).
  Convert to real pixels only at render time (`fixed_point_to_pixel`).
- **Screen/velocity coordinate convention**: +x = right, +y = **down** (not up —
  standard top-left-origin screen space, opposite of math/graph convention).
- **`move_sprite()`/`move_metasprite_ex()`/`move_metasprite_flipx()` hardware
  offset**: GB sprite coordinates are offset by `+8` (x) / `+16` (y) from the
  visible screen position (hardware reserves margin for sprites scrolling
  on/off-screen). Handled once in `move_sprite_to_position`/
  `move_metasprite_ref` (the shared helper behind both
  `move_metasprite_to_position` and `move_animated_metasprite_to_position`),
  not something callers need to think about.
- **Opposing D-pad directions cancel to 0** rather than one arbitrarily winning
  (`compute_velocity_from_joypad` checks e.g. `left && !right`). Note: this
  couldn't actually be exercised/tested — Emulicious (like real hardware) never
  reports both Left+Right pressed simultaneously, so the cancellation logic is
  correct but currently unverified by observed behavior.
- **Struct composition**: value-embed sub-structs (`Player.position` is a
  `Position`, not a `Position *`) — no heap/malloc on this platform, so a
  pointer member would just add indirection with no ownership benefit. This
  does *not* apply to references into shared/const ROM data (e.g.
  `MetaspriteRef.metasprite` is a `const metasprite_t *` into a generated,
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

- **Enemies exist but don't do anything yet.** `seed_enemies` spawns
  `Context.enemies` at a single hardcoded position (`INITIAL_ENEMY_POSITION_X/Y`
  in `main.c`) — no per-enemy spawn points, and nothing in `process_frame`
  calls `apply_enemy_velocity`/`update_enemy_sprite` yet, so enemies are
  currently inert (positioned, tiled, and paletted, but never moved or
  re-synced to their sprite after the initial placement in
  `initialize_enemy`).
- **No tracking of how many `Context.enemies` slots are actually active.**
  `Enemy` is value-embedded (`Enemy enemies[MAX_ENEMIES]`), so seeding fewer
  than `MAX_ENEMIES` doesn't leave "empty"/null slots — the untouched tail
  entries are zero-initialized structs (a quirk of C's designated-initializer
  rule for the omitted `.enemies` field in `Context context = { ... }`),
  meaning `sprite_num = 0` — the same OAM slot as `PLAYER_SPRITE_SLOT`. Safe
  today only because nothing iterates the full `MAX_ENEMIES` range yet; once
  something does (e.g. a future per-frame enemy update loop), it needs either
  an explicit active-count field on `Context` or always seeding all
  `MAX_ENEMIES` slots — see the "Enemies" section above.
- **Direction-facing is wired up; walk-cycle animation still isn't.**
  `update_player_sprite` → `move_animated_metasprite_to_position` picks the
  right `MetaspriteAnimationFrames` for `player->direction`, but always reads
  index 0 (`metasprite.frames[direction].refs[0]`) — there's no per-instance
  animation-frame counter on `Player` yet, and `res/player.png` doesn't have
  walk-cycle art to select between yet anyway (see "Metasprites" above).
  Adding it needs: (1) walk-cycle frames added to `res/player.png` per
  direction (or accept fewer directions animating than others — see
  `METASPRITE_MAX_ANIMATION_FRAMES`'s "pad with idle" convention), (2) an
  animation-frame/timer pair on `Player` (an earlier attempt at this lived on
  `Player` as `animation_frame`/`animation_timer`, advanced in a dedicated
  `update_player_animation` step between `apply_velocity` and
  `update_player_sprite` — reasonable shape, just reverted because it was
  built before direction-facing existed and animated off velocity alone), and
  (3) `move_animated_metasprite_to_position` (or its caller) indexing
  `refs[animation_frame]` instead of the hardcoded `refs[0]`.
- **No collision detection** — `apply_velocity` applies velocity unconditionally.
  Planned approach (see Architecture above): candidate position → resolve
  against world (per-axis, not combined, to allow wall-sliding) → commit →
  sync sprite. Not yet implemented.
- **`Metasprite`/`move_metasprite_to_position` (the non-directional,
  single-frame path) currently have no callers** — `Player` moved fully to
  `AnimatedMetasprite`. Kept intentionally for a future entity that's
  multi-tile but doesn't need direction-facing (see "Metasprites" above).
- **Sub-pixel movement is implemented but movement is simple** — constant
  velocity per frame while a direction is held, no acceleration/friction, no
  diagonal-speed normalization (diagonal movement is faster than cardinal).
- **`hide_sprites_range` cleanup in `move_metasprite_to_position` is currently
  inert** — correct and scoped per-entity (see "Metasprites" above), but has
  nothing to do until sprite counts actually vary frame-to-frame (animation,
  despawning, or variable entity counts).

## SDCC/GBDK gotchas encountered (useful if debugging build warnings)

- **`png2asset` re-sorts a PNG's palette by color frequency by default**,
  discarding the index order in the PNG's own `PLTE` chunk — pass
  `-keep_palette_order` to preserve it. This matters whenever pixel *index*
  (not the PNG's own embedded color) is what determines the rendered color —
  e.g. `res/enemy.png` deliberately reuses the player's CGB palette slot
  instead of loading its own (see "Enemies" above); without
  `-keep_palette_order`, the generic `$(RESOBJDIR)/%.c` pattern rule
  resorted its colors and silently broke which pixels rendered as which
  color.

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
