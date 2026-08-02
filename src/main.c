#include <gb/gb.h>
#include <stdint.h>

#include "player.h"
#include "splash.h"
#include "utils/util.h"

#define PLAYER_SPRITE_TILE_START_INDEX 0 // length 1

#define PLAYER_SPRITE_SLOT 0

void initialize_sprites(void)
{
    // Load the tile data into VRAM's sprite tile table once
    initialize_player_sprite_tiles(PLAYER_SPRITE_TILE_START_INDEX);

    // Assign that tile to a sprite slot (hardware supports 40 slots, 0–39) once
    set_sprite_tile(PLAYER_SPRITE_SLOT, PLAYER_SPRITE_TILE_START_INDEX); 

    // Turn sprites on once
    SHOW_SPRITES;
}

void main(void)
{
    splash();
    reset_screen();

    initialize_sprites();

    Position player_initial_position = { .x = 1216, .y = 1088 };
    Player player = initialize_player(PLAYER_SPRITE_SLOT, player_initial_position);

    // Loop forever
    while(1) {

		// Game main loop processing goes here


		// Done processing, yield CPU and wait for start of next frame
        vsync();
    }
}
