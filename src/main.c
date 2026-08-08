#include <gb/gb.h>
#include <stdint.h>

#include "joypad.h"
#include "player.h"
#include "splash.h"
#include "sprites.h"
#include "utils/util.h"
#include <res/player.h>

void initialize_player_sprite(void)
{
    // load the tile data into VRAM's sprite tile table once
    set_sprite_data(PLAYER_SPRITE_TILE_START_INDEX, player_TILE_COUNT, player_tiles);
    // Load the real sprite colors once — without this, sprites use the
    // uninitialized/default palette and render faded/grayscale.
    update_sprite_pallete(player_palettes);
}

void initialize_sprites(void)
{
    HIDE_SPRITES; // temporarily turn off the sprites layer
    SPRITES_8x8; // sprites can be 8x8 or 8x16 so need to ensure sprite size register is initialized
                 // correctly

    initialize_player_sprite();

    SHOW_SPRITES; // turn on sprites layer, once
}

Player create_player(void)
{
    Position player_initial_position = { .x = 1216, .y = 1088 };
    Metasprite metasprite = {
        .metasprite = player_metasprites[0],
        .sprite_num = PLAYER_SPRITE_SLOT,
        .start_tile_index = PLAYER_SPRITE_TILE_START_INDEX,
        .base_props = DEFAULT_METASPRITE_BASE_PROP,
        .max_sprite_count = PLAYER_SPRITE_MAX_COUNT,
    };
    Player player = initialize_player(player_initial_position, metasprite);
    return player;
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
    Player player = create_player();

    // Loop forever
    while (1) {
        // Game main loop processing goes here
        process_frame(&player);

        // Done processing, yield CPU and wait for start of next frame
        vsync();
    }
}
