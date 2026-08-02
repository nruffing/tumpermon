#include "splash.h"

#include <gb/gb.h>
#include <gb/cgb.h>
#include <gb/drawing.h>

#include "utils/util.h"
#include "utils/apa_util.h"
#include "colors.h"


void splash(void)
{
    do_apa_mode();

    const palette_color_t splash_palette[4] = {
        COLOR_WHITE, // WHITE  -> white
        COLOR_LIGHTGRAY,   // LTGREY -> red
        COLOR_ORANGE,  // DKGREY -> blue
        COLOR_BLACK, // BLACK  -> black
    };
    update_color_pallete(splash_palette);

    // foreground = COLOR_ORANGE, background = COLOR_BLACK
    color(DKGREY, BLACK, SOLID);

    // set background to COLOR_BLACK with COLOR_ORANGE border
    box(0, 0, (uint8_t)(GRAPHICS_WIDTH - 1), (uint8_t)(GRAPHICS_HEIGHT - 1), M_FILL);

    const char *title = "TUMPERMON";
    gotogxy(get_centered_x(title), 4);
    gprint((char *)title);
}