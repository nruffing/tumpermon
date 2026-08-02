
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

