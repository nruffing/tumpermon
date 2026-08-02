#include <gb/gb.h>
#include <stdint.h>

#include "splash.h"
#include "utils/util.h"

void main(void)
{
    splash();
    reset_screen();

    // Loop forever
    while(1) {

		// Game main loop processing goes here


		// Done processing, yield CPU and wait for start of next frame
        vsync();
    }
}
