#include "util.h"

#include <gb/gb.h>

bool is_cgb(void)
{
    return _cpu == CGB_TYPE;
}

void update_color_pallete(const palette_color_t *rgb_data)
{
    if (is_cgb()) {        
        set_bkg_palette(BKGF_CGB_PAL0, 1, rgb_data);
    } else {
        BGP_REG = DMG_PALETTE(DMG_WHITE, DMG_LITE_GRAY, DMG_DARK_GRAY, DMG_BLACK);
    }
}