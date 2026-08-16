# TUMPERMON

A Game Boy homebrew game built with [GBDK-2020](https://github.com/gbdk-2020/gbdk-2020),
started from GBDK's `template-minimal` example. Currently has a splash screen and a
player character that can be moved around with the D-pad.

The current focus is building the game engine mostly as a learning exercise. Game content will be a focus later. The goal is to end up with a game engine that could be used for various types of games and additional more opinionated support for top-down, top-down tile-based overworld RPG style games. There is no attempt at creating a separate library for this engine yet but code architecture decisions should favor doing that easily in the future.

The Makefile automatically detects and compiles new source files as long as they're
placed in `src/` (or one subfolder deep, e.g. `src/utils/`).

See `CLAUDE.md` for architecture notes, coding conventions, and known gaps.

## Setup

1. **Install GBDK-2020**, the Game Boy dev toolchain this project builds
   against: https://github.com/gbdk-2020/gbdk-2020/releases

2. **Clone this repo two directories deep inside your GBDK install**, e.g.
   `<gbdk-root>/git/tumpermon`. The Makefile defaults `GBDK_HOME` to `../../`
   (relative to this project), so it expects that layout out of the box.

   If you'd rather keep the repo somewhere else, point `GBDK_HOME` at your
   GBDK install instead, e.g.:

   ```
   make GBDK_HOME=/path/to/gbdk
   ```

3. **(Optional) Install an emulator** to run the built ROM, e.g.
   [Emulicious](https://emulicious.net/).

4. **(Optional, VS Code) Get IntelliSense working.** Opening this folder in
   VS Code will prompt you to install the recommended extensions
   (`.vscode/extensions.json`) — accept that prompt and IntelliSense
   (`.vscode/c_cpp_properties.json`) will pick up GBDK's headers
   automatically, assuming the same two-directories-deep layout from step 2.

5. **(Optional) Install `clang-format`** for code formatting, e.g.
   `brew install clang-format` on macOS. Formatting follows `.clang-format`
   at the repo root; with the recommended VS Code extensions installed
   (step 4), `.vscode/settings.json` formats C files on save automatically.
   To format everything from the command line:
   ```
   find src -name "*.c" -o -name "*.h" | xargs clang-format -i
   ```

6. **Enable the pre-commit format hook**, which auto-runs `clang-format` on
   staged `.c`/`.h` files (via `.githooks/pre-commit`) before each commit.
   `core.hooksPath` isn't set by cloning, so run this once per clone:
   ```
   git config core.hooksPath .githooks
   ```

## Building

The Makefile assumes GBDK is installed two directories up (`../../`). If your
GBDK install is elsewhere, set `GBDK_HOME` to point at it.

**macOS / Linux:**

```
make
```

**Windows:**

```
compile.bat
```

Either produces `Tumpermon.gb` in this directory. Run `make clean` to remove
build artifacts.

## Running

Open `Tumpermon.gb` in an emulator, e.g. [Emulicious](https://emulicious.net/):

```
open -a Emulicious Tumpermon.gb
```

or drag the file onto the Emulicious app / window.

## Flashing to a real cartridge

To play `Tumpermon.gb` on real Game Boy hardware, flash it to a flash
cartridge using a [GBxCart RW](https://www.gbxcart.com/) device:

1. **Install [FlashGBX](https://www.gbxcart.com/)**, the flashing software
   for GBxCart RW (Windows, Mac, and Linux). On Windows you'll also need the
   CH340/CH341 USB driver, linked from the same site.

   On macOS, since FlashGBX isn't notarized/signed, Gatekeeper will block it
   from opening after you drag it to `/Applications`. Remove the quarantine
   attribute to allow it to run:

   ```
   xattr -dr com.apple.quarantine /Applications/FlashGBX.app
   ```

2. **Connect the GBxCart RW device** to your computer via USB, and insert
   your flash cartridge into it.

3. **Open FlashGBX** and select the ROM file to flash — point it at the
   `Tumpermon.gb` produced by `make`/`compile.bat`.

4. **Select your cartridge/mapper type** if FlashGBX doesn't auto-detect it
   (this depends on which flash cart you're using).

5. **Write the ROM**, then let FlashGBX verify the write completed
   successfully.

6. Insert the cartridge into a real Game Boy / Game Boy Color and power on.

For cart-specific mapper settings or troubleshooting, gbxcart.com points to
their Discord server for support.

This was confirmed to work with
 * [FlashGBX v5.0.1](https://github.com/lesserkuma/FlashGBX/releases/tag/5.0.1)
 * [GBxCart RW v1.4a/b/c](https://retrogamerepairshop.com/collections/insidegadgets/products/gbxcart-rw-gameboy-gbc-gba-cart-reader-writer-flasher)
 * Firmware R42+L12 (2024-06-28T14:25:31-07:00)
   * FlashGBX will prompt to update firmware if there is a new version available.
 * Game Boy 2MB, 32KB FRAM MBC3 with RTC Flash Cartridge by HDR
 * on macOS 26.5.2
 * ModRetro Chromatic [1st Edition] (firmware updated to v4.2)

## Creating Sprites/Tiles

### gb-sprite

Sprites were creating using [gb-sprite](https://github.com/nruffing/gb-sprite) which was developed in-parallel to this game. 

### Game Boy Tile Tool

`gb-sprite` was inspired by [Game Boy Tile Tool](https://nathanheffley.itch.io/game-boy-tile-tool) which is also a good option. `gb-sprite` was aiming to be more opinionated on the workflow that seemed to work the best for me as I was learning sprite design.

After installing `Game Boy Tile Tool` you need to run the following on macOS after install to Applications directory.
```
xattr -dr com.apple.quarantine "/Applications/Gameboy Tile Tool.app"
```

## Adding a New Metasprite

Multi-tile sprites (anything bigger than a single 8x8/8x16 tile — e.g. the
player) are built as GBDK metasprites, generated from a PNG via `png2asset`.
Single-tile entities (e.g. a simple projectile) don't need any of this —
just use `move_sprite_to_position` from `kinematics.h` directly.

1. **Create an indexed-color PNG** and drop it in `res/`, e.g. `res/enemy.png`.
   - Must be **indexed/paletted** (not RGB/RGBA) — export as such from your
     sprite tool. `gb-sprite` and Game Boy Tile Tool (above) both do this.
   - Dimensions must be a multiple of 8x8 (e.g. 16x16 for a 2x2-tile sprite).
   - For multiple frames in one file — whether animation frames or sprite
     variants like direction-facing — stack them at a consistent frame size
     and pass `-sh <frame height>` (frames stacked vertically) or
     `-sw <frame width>` (frames stacked horizontally) — see step 2. Each
     frame becomes an entry in the generated `..._metasprites[]` array, in
     stacking order. `res/player.png` is the example to follow: a 48x16 sheet
     of three 16x16 direction-facing frames (down, up, left/right) stacked
     horizontally (`-sw 16`), giving `player_metasprites[0..2]` — no
     walk-cycle animation frames yet, just one frame per facing.
   - ≤4 colors (2bpp) keeps it on a single CGB sprite palette; more needs
     `-max_palettes`/multiple `set_sprite_palette` slots.

2. **The Makefile picks up single-frame pngs automatically** via the generic
   `$(RESOBJDIR)/%.c` pattern rule — every `.png` in `res/` gets converted by
   `png2asset` into `obj/res/<name>.c`/`.h` as part of `make`, no Makefile
   changes needed. **Multi-frame pngs need an explicit per-file rule** adding
   `-sw`/`-sh` (the generic rule can't know your frame size) — add one above
   the generic rule, following `$(RESOBJDIR)/player.c`'s as the pattern. If
   your sprite isn't 8x8-tile hardware sprites (i.e. using 8x16 mode), adjust
   the `png2asset` flags similarly — currently `-spr8x8 -noflip`, matching the
   player's art.

3. **`#include <res/<name>.h>`** in the `.c` file that uses it. This gives
   you (using `enemy.png` as an example):
   ```c
   enemy_TILE_COUNT             // number of 8x8 tiles
   enemy_tiles[]                // raw tile pixel data, for set_sprite_data
   enemy_palettes[]             // palette_color_t[4], for set_sprite_palette
   enemy_metasprites[N]         // one metasprite_t* per animation frame
   ```

4. **Reserve VRAM tile indices and an OAM sprite slot range** for it in
   `sprites.h`, following the `PLAYER_SPRITE_*` pattern — each metasprite
   needs `..._TILE_COUNT` contiguous VRAM tile slots and that many
   contiguous OAM hardware sprite slots (Game Boy has 40 total, shared
   across everything on screen — budget accordingly as more sprites are
   added).

5. **Load tile data + palette once**, e.g. in `initialize_sprites()` in
   `main.c`:
   ```c
   set_sprite_data(ENEMY_SPRITE_TILE_START_INDEX, enemy_TILE_COUNT, enemy_tiles);
   update_sprite_pallete(enemy_palettes);
   ```

6. **Build a `Metasprite` value** (see `sprites.h`) pointing at the frame you
   want, and move it with `move_metasprite_to_position` from `kinematics.c`:
   ```c
   Metasprite metasprite = {
       .ref = { .metasprite = enemy_metasprites[0], .flip_x = false },
       .metadata = {
           .sprite_num = ENEMY_SPRITE_SLOT,
           .start_tile_index = ENEMY_SPRITE_TILE_START_INDEX,
           .max_sprite_count = ENEMY_SPRITE_MAX_COUNT,
       },
   };
   move_metasprite_to_position(metasprite, position);
   ```
   This is for an entity with one fixed frame (no direction-facing, no
   animation). For a directional entity like the player — several frames
   selected by facing, optionally several more per facing for a walk cycle —
   use `AnimatedMetasprite` instead: one `MetaspriteRef` per `Direction`,
   each built into a `MetaspriteAnimationFrames` (pad the unused frame slots
   with a repeated idle ref via `pad_metasprite_animation_frames` from
   `utils/metasprite_util.h` if you don't have distinct walk frames for every
   direction yet), and move it with `move_animated_metasprite_to_position`
   (takes a `Direction` in addition to `Position`). `player.c`'s
   `create_player_metasprite` is the example to follow — it also shows the
   `flip_x` mirroring trick (reusing one frame for two opposite-facing
   directions instead of drawing both).

7. **Rebuild** (`make`) — `obj/res/` is regenerated from the PNGs each time
   and is gitignored, so nothing under it needs to be committed.