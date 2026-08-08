#include "util.h"

#include <gb/gb.h>
#include <gbdk/console.h>

bool is_cgb(void)
{
    return _cpu == CGB_TYPE;
}

void reset_to_dmg_color_pallete(void)
{
    BGP_REG = DMG_PALETTE(DMG_WHITE, DMG_LITE_GRAY, DMG_DARK_GRAY, DMG_BLACK);
}

void update_color_pallete(const palette_color_t *rgb_data)
{
    if (is_cgb()) {
        set_bkg_palette(BKGF_CGB_PAL0, 1, rgb_data);
    } else {
        reset_to_dmg_color_pallete();
    }
}

void reset_color_pallete(void)
{
    if (is_cgb()) {
        set_default_palette();
    } else {
        reset_to_dmg_color_pallete();
    }
}

void update_sprite_pallete(const palette_color_t *rgb_data)
{
    if (is_cgb()) {
        set_sprite_palette(OAMF_CGB_PAL0, 1, rgb_data);
    } else {
        OBP0_REG = DMG_PALETTE(DMG_WHITE, DMG_LITE_GRAY, DMG_DARK_GRAY, DMG_BLACK);
    }
}

void reset_screen(void)
{
    DISPLAY_OFF;
    cls();

    // Blank out VRAM tile pattern data left behind by APA drawing —
    // cls() only resets the tile map, not the underlying tile patterns.
    const uint8_t blank_tile[16] = { 0 };
    for (uint16_t i = 0; i < 256; i++) {
        set_bkg_data((uint8_t)i, 1, blank_tile);
    }

    DISPLAY_ON;
}