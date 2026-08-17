#
# Simple Makefile that compiles all .c and .s files in the src/ folder
#

# If you move this project you can change the directory 
# to match your GBDK root directory (ex: GBDK_HOME = "C:/GBDK/"
ifndef GBDK_HOME
	GBDK_HOME = ../../
endif

LCC       = $(GBDK_HOME)/bin/lcc
PNG2ASSET = $(GBDK_HOME)/bin/png2asset

#GBDK_DEBUG = ON
ifdef GBDK_DEBUG
	LCCFLAGS += -debug -v
	# Makefile variables aren't visible to C — this is what actually lets
	# `#ifdef GBDK_DEBUG` work in .c/.h files (e.g. to compile out
	# EMU_printf calls in a non-debug build).
	CFLAGS += -DGBDK_DEBUG
endif

# Make the ROM CGB compatible (but not exclusive)
LCCFLAGS += -Wm-yc


# You can set the name of the .gb ROM file here
PROJECTNAME    = Tumpermon

BINS	    = $(PROJECTNAME).gb
CSOURCES   := $(wildcard src/*.c src/*/*.c)
ASMSOURCES := $(wildcard src/*.s src/*/*.s)

# Metasprite pngs in res/ -> generated .c/.h in obj/res/
# (single-frame pngs use their own width/height automatically, so no -sw/-sh needed.
# Multi-frame pngs need an explicit per-file rule below passing -sw/-sh so
# png2asset splits the sheet into separate metasprite frames.)
RESDIR    = res
RESOBJDIR = obj/res
METAPNGS  := $(wildcard $(RESDIR)/*.png)
METASRCS  := $(patsubst $(RESDIR)/%.png,$(RESOBJDIR)/%.c,$(METAPNGS))

CSOURCES += $(METASRCS)
CFLAGS   += -Iobj

all:	$(BINS)

compile.bat: Makefile
	@echo "REM Automatically generated from Makefile" > compile.bat
	@make -sn | sed y/\\//\\\\/ | sed s/mkdir\ \-p/mkdir/ | grep -v make >> compile.bat

# res/player.png is a 48x16 sheet of three 16x16 direction-facing frames
# stacked horizontally, in order: down, up, left/right (mirror for the
# opposite side). No walk-cycle frames yet — one static frame per facing. -sw 16
# tells png2asset to split it into three metasprite frames
# (player_metasprites[0..2]) instead of one 48-wide frame. This explicit rule
# overrides the generic pattern rule below for this file.
$(RESOBJDIR)/player.c:	$(RESDIR)/player.png
	@mkdir -p $(RESOBJDIR)
	$(PNG2ASSET) $< -sw 16 -spr8x8 -noflip -c $@

# res/font.png is a background/window tileset (menu text), not a sprite sheet —
# omit -spr8x8 so png2asset emits plain BG/window-format tile data
# (font_tiles/font_TILE_COUNT) instead of metasprite structures. Placeholder
# copied from examples/cross-platform/text_basic/res/Font.png; swap for real
# art later. This explicit rule overrides the generic pattern rule below.
$(RESOBJDIR)/font.c:	$(RESDIR)/font.png
	@mkdir -p $(RESOBJDIR)
	$(PNG2ASSET) $< -c $@ -map -keep_palette_order -noflip

# Convert metasprite pngs to C source before they're needed as CSOURCES
$(RESOBJDIR)/%.c:	$(RESDIR)/%.png
	@mkdir -p $(RESOBJDIR)
	$(PNG2ASSET) $< -spr8x8 -noflip -c $@

# Compile and link all source files in a single call to LCC
$(BINS):	$(CSOURCES) $(ASMSOURCES)
	$(LCC) $(LCCFLAGS) $(CFLAGS) -o $@ $(CSOURCES) $(ASMSOURCES)

clean:
	rm -f *.o *.lst *.map *.gb *.ihx *.sym *.cdb *.adb *.asm *.noi *.rst
	rm -rf $(RESOBJDIR)

