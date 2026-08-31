#include <gb/gb.h>
#include <stdint.h>

#include "context.h"
#include "enemy.h"
#include "joypad.h"
#include "kinematics.h"
#include "menu.h"
#include "menus.h"
#include "player.h"
#include "splash.h"
#include "sprites.h"
#include "utils/rand_util.h"
#include "utils/util.h"
#include <res/enemy.h>
#include <res/font.h>
#include <res/player.h>

#define INITIAL_PLAYER_DIRECTION DIRECTION_DOWN
#define INITIAL_PLAYER_POSITION_X 1216
#define INITIAL_PLAYER_POSITION_Y 1088

#define ENEMY_COUNT 2

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

    set_sprite_data(ENEMY_TILE_START_INDEX, enemy_TILE_COUNT, enemy_tiles);

    SHOW_SPRITES; // turn on sprites layer, once
}

void initialize_font(void)
{
    // Loads the font tileset (res/font.png) into the shared BG/window tile
    // pool at FONT_TILE_START_INDEX (sprites.h) once, before any
    // draw_win_text call.
    set_win_data(FONT_TILE_START_INDEX, font_TILE_COUNT, font_tiles);
}

Player create_player(void)
{
    Position player_initial_position = { .x = INITIAL_PLAYER_POSITION_X,
                                         .y = INITIAL_PLAYER_POSITION_Y };
    AnimatedMetasprite metasprite = create_player_metasprite();
    Player player =
        initialize_player(player_initial_position, INITIAL_PLAYER_DIRECTION, metasprite);
    return player;
}

void reset_player(Context *context)
{
    context->player->position.x = INITIAL_PLAYER_POSITION_X;
    context->player->position.y = INITIAL_PLAYER_POSITION_Y;
    context->player->direction = INITIAL_PLAYER_DIRECTION;
    update_player_sprite(context->player);
}

void seed_enemies(Enemy *enemies, uint8_t enemy_count)
{
    for (uint8_t i = 0; i < enemy_count; i++) {
        Position initial_position = { .x = rand_range(MIN_POSITION_X, MAX_POSITION_X),
                                      .y = rand_range(MIN_POSITION_Y, MAX_POSITION_Y) };
        enemies[i] = initialize_enemy(i, initial_position);
    }
}

void reset_enemies(Context *context)
{
    for (uint8_t i = 0; i < context->enemy_count; i++) {
        Enemy *enemy = &context->enemies[i];
        enemy->position.x = rand_range(MIN_POSITION_X, MAX_POSITION_X);
        enemy->position.y = rand_range(MIN_POSITION_Y, MAX_POSITION_Y);
        update_enemy_sprite(enemy);
    }
}

void show_menu_on_context(Context *context)
{
    context->menu->selected_index = 0;
    show_menu(context->menu->open_menu, context->menu->selected_index);
}

void hide_menu_on_context(Context *context)
{
    context->menu->open_menu = NULL;
    hide_menu();
}

void handle_pause(Context *context)
{
    if (context->joypad_state.start.is_just_pressed) {
        context->is_paused = !context->is_paused;
        if (context->is_paused) {
            context->menu->open_menu = &pause_menu;
            show_menu_on_context(context);
        } else {
            hide_menu_on_context(context);
        }
    }
}

void handle_menu_item_select(Context *context)
{
    const Menu *menu = context->menu->open_menu;
    MenuOption option = menu->options[context->menu->selected_index];

    switch (menu->id) {
    case PAUSE_MENU_ID:
        if (option.id == PAUSE_MENU_RESTART_ID) {
            reset_player(context);
            reset_enemies(context);
        }
        // no state changes required for resume menu option
        context->is_paused = false;
        break;
    }

    hide_menu_on_context(context);
}

void handle_menu(Context *context)
{
    if (context->menu->open_menu == NULL) {
        return;
    }

    if (context->joypad_state.down.is_just_pressed) {
        if (context->menu->selected_index == context->menu->open_menu->option_count - 1) {
            return;
        }
        uint8_t previous_index = context->menu->selected_index;
        context->menu->selected_index += 1;
        select_menu_item(previous_index, context->menu->selected_index);
    } else if (context->joypad_state.up.is_just_pressed) {
        if (context->menu->selected_index == 0) {
            return;
        }
        uint8_t previous_index = context->menu->selected_index;
        context->menu->selected_index -= 1;
        select_menu_item(previous_index, context->menu->selected_index);
    } else if (context->joypad_state.select.is_just_pressed) {
        // same as down but roll around
        uint8_t previous_index = context->menu->selected_index;
        context->menu->selected_index =
            context->menu->selected_index == context->menu->open_menu->option_count - 1
                ? 0
                : context->menu->selected_index + 1;
        select_menu_item(previous_index, context->menu->selected_index);
    } else if (context->joypad_state.a.is_just_pressed) {
        handle_menu_item_select(context);
    }

    blink_menu_cursor(context->menu->selected_index, context->tick);
}

void process_enemies(Context *context)
{
    for (uint8_t i = 0; i < context->enemy_count; i++) {
        Enemy *enemy = &context->enemies[i];
        NodeContext node_context = { .game_context = context, .enemy = enemy };
        enemy->behavior_tree->tick(enemy->behavior_tree, &node_context);
        apply_enemy_velocity(enemy);
        update_enemy_sprite(enemy);
    }
}

void process_frame(Context *context)
{
    context->tick++;

    context->joypad_state = process_joypad(context->tick);

    handle_pause(context);
    handle_menu(context);
    if (context->is_paused) {
        return;
    }

    Velocity velocity = compute_velocity_from_joypad(&context->joypad_state, context->kinematics);
    apply_player_velocity(context->player, velocity);
    update_player_sprite(context->player);

    process_enemies(context);
}

void main(void)
{
    uint16_t splash_tick = 0;
    show_splash();
    while (1) {
        JoypadState state = process_joypad(splash_tick);
        if (state.start.is_just_pressed) {
            break;
        }
        splash_tick++;
        vsync(); // wait for next frame
    }
    hide_splash();
    reset_screen();
    initialize_random(splash_tick);

    initialize_sprites();
    initialize_font();

    Player player = create_player();
    KinematicBehaviorContext kinematics = { .allow_diagonal_movement = false };
    MenuContext menus = { .open_menu = NULL, .selected_index = 0 };
    uint8_t enemy_count = ENEMY_COUNT > MAX_ENEMIES ? MAX_ENEMIES : ENEMY_COUNT;
    Context context = { .tick = 0,
                        .is_paused = false,
                        .menu = &menus,
                        .player = &player,
                        .kinematics = &kinematics,
                        .enemy_count = enemy_count };
    seed_enemies(context.enemies, enemy_count);

    // Loop forever
    while (1) {
        // Game main loop processing goes here
        process_frame(&context);

        // Done processing, yield CPU and wait for start of next frame
        vsync();
    }
}
