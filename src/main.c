#include <gb/gb.h>
#include <stdint.h>

#include "context.h"
#include "joypad.h"
#include "kinematics.h"
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
    AnimatedMetasprite metasprite = create_player_metasprite();
    Player player = initialize_player(player_initial_position, metasprite);
    return player;
}

void handle_pause(Context *context)
{
    if (context->joypad_state.start.is_just_pressed) {
        context->is_paused = !context->is_paused;
    }
}

void process_frame(Context *context)
{
    context->tick++;

    context->joypad_state = process_joypad(context->tick);
    handle_pause(context);
    if (context->is_paused) {
        return;
    }

    Velocity velocity = compute_velocity_from_joypad(&context->joypad_state, context->kinematics);
    apply_velocity(context->player, velocity);
    update_player_sprite(context->player);
}

void main(void)
{
    splash();
    reset_screen();

    initialize_sprites();

    Player player = create_player();
    KinematicBehaviorContext kinematics = { .allow_diagonal_movement = false };
    Context context = { .tick = 0,
                        .is_paused = false,
                        .player = &player,
                        .kinematics = &kinematics };

    // Loop forever
    while (1) {
        // Game main loop processing goes here
        process_frame(&context);

        // Done processing, yield CPU and wait for start of next frame
        vsync();
    }
}
