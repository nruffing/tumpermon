
An minimal template project with a Makefile that only compiles files in the same directory

The Makefile will automatically detect and compile new source files as long 
as they are placed in the same directory as the Makefile

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

Sprites were creating using [gb-sprite](https://github.com/nruffing/gb-sprite) which was developed in-parallel. 

`gb-sprite` was inspired by [Game Boy Tile Tool](https://nathanheffley.itch.io/game-boy-tile-tool) which is also a good option. `gb-sprite` was aiming to be more opinionated on the workflow that seemed to work the best for me as I was learning sprite design.

After installing `Game Boy Tile Tool` you need to run the following on macOS after install to Applications directory.
```
xattr -dr com.apple.quarantine "/Applications/Gameboy Tile Tool.app"
```