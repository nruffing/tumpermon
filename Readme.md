
An minimal template project with a Makefile that only compiles files in the same directory

The Makefile will automatically detect and compile new source files as long 
as they are placed in the same directory as the Makefile

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

