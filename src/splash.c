#include "splash.h"

#include <stdbool.h>
#include <gb/gb.h>
#include <gb/cgb.h>
#include <gb/drawing.h>

#include "utils/util.h"
#include "utils/apa_util.h"
#include "colors.h"

uint8_t joypad_current=0, joypad_previous=0;

void wait_for_start(void) {
    while(1) {
        joypad_previous = joypad_current;
        joypad_current = joypad();

        // check if button is pressed now and was not before (i.e. button down edge-detection)
        bool is_pressed = joypad_current & J_START;
        if (is_pressed) {
            bool was_pressed = joypad_previous & J_START;
            if (!was_pressed) {
                break;
            }
        }

        vsync(); // wait for next frame
    }
}

void splash(void)
{
    do_apa_mode();

    const palette_color_t splash_palette[4] = {
        COLOR_WHITE,     // WHITE  -> white
        COLOR_LIGHTGRAY, // LTGREY -> red
        COLOR_ORANGE,    // DKGREY -> blue
        COLOR_BLACK,     // BLACK  -> black
    };
    update_color_pallete(splash_palette);

    // foreground = COLOR_ORANGE, background = COLOR_BLACK
    color(DKGREY, BLACK, SOLID);

    // set background to COLOR_BLACK with COLOR_ORANGE border
    box(0, 0, (uint8_t)(GRAPHICS_WIDTH - 1), (uint8_t)(GRAPHICS_HEIGHT - 1), M_FILL);

    const char *title = "TUMPERMON";
    print_centered(title, 5);
    const char *press_start = "Press Start";
    print_centered(press_start, 13);

    wait_for_start();
    end_apa_mode();
}
