#include "splash.h"

#include <gb/gb.h>
#include <gb/cgb.h>
#include <gb/drawing.h>

#include "utils/util.h"
#include "utils/apa_util.h"


void splash(void)
{
    do_apa_mode();

    const palette_color_t splash_palette[4] = {
        RGB(31, 31, 31), // WHITE  -> white
        RGB(31, 0,  0),  // LTGREY -> red
        RGB(0,  0,  31), // DKGREY -> blue
        RGB(0,  0,  0),  // BLACK  -> black
    };
    update_color_pallete(splash_palette);

    color(BLACK, WHITE, SOLID);

    const char *title = "TUMPERMON";
    gotogxy(get_centered_x(title), 4);
    gprint((char *)title);
}