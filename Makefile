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

# GBDK_DEBUG = ON
ifdef GBDK_DEBUG
	LCCFLAGS += -debug -v
endif

# Make the ROM CGB compatible (but not exclusive)
LCCFLAGS += -Wm-yc


# You can set the name of the .gb ROM file here
PROJECTNAME    = Tumpermon

BINS	    = $(PROJECTNAME).gb
CSOURCES   := $(wildcard src/*.c src/*/*.c)
ASMSOURCES := $(wildcard src/*.s src/*/*.s)

# Metasprite pngs in res/ -> generated .c/.h in obj/res/
# (single-frame pngs use their own width/height automatically, so no -sw/-sh needed)
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

