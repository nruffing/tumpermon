#include <gb/gb.h>
#include <stdint.h>

#include "joypad.h"
#include "player.h"
#include "splash.h"
#include "sprites.h"
#include "utils/util.h"

void initialize_sprites(void)
{
    // Load the tile data into VRAM's sprite tile table once
    initialize_player_sprite_tiles(PLAYER_SPRITE_TILE_START_INDEX);

    // Assign that tile to a sprite slot (hardware supports 40 slots, 0–39) once
    set_sprite_tile(PLAYER_SPRITE_SLOT, PLAYER_SPRITE_TILE_START_INDEX); 

    // Turn sprites on once
    SHOW_SPRITES;
}

void process_frame(Player *player)
{
    JoypadState state = process_joypad();
    Velocity velocity = compute_velocity_from_joypad(state);
    apply_velocity(player, velocity);
    update_player_sprite(player);
}

void main(void)
{
    splash();
    reset_screen();

    initialize_sprites();

    Position player_initial_position = { .x = 1216, .y = 1088 };
    Player player = initialize_player(player_initial_position);

    // Loop forever
    while(1) {
		// Game main loop processing goes here
        process_frame(&player);

		// Done processing, yield CPU and wait for start of next frame
        vsync();
    }
}
