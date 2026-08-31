#include "menu.h"

#include <gb/gb.h>
#include <stdbool.h>

#include "utils/tick_util.h"
#include "window.h"

// Tile row/column the title and first option are drawn at, in window-tile
// coordinates (not screen pixels — see get_win_position_x for that offset).
#define MENU_TITLE_ROW 1
#define MENU_TITLE_COL 1
#define MENU_OPTIONS_START_ROW 3
#define MENU_OPTION_COL 2
#define MENU_CURSOR_COL (MENU_OPTION_COL - 1)

#define MENU_CURSOR_COLUMN_SELECTED ">"
#define MENU_CURSOR_COLUMN_NOT_SELECTED " "

// How many ticks the cursor stays on/off per half-cycle of its blink — see
// tick_toggle_state (tick_util.h). ~30 ticks is half a second at ~60fps.
#define MENU_CURSOR_BLINK_PERIOD_TICKS 30

static uint8_t get_row_for_menu_item_index(uint8_t menu_item_index)
{
    return (uint8_t)(MENU_OPTIONS_START_ROW + menu_item_index);
}

void show_menu(const Menu *menu, uint8_t selected_index)
{
    HIDE_SPRITES;

    move_win_fullscreen();

    clear_win();
    draw_win_text(MENU_TITLE_COL, MENU_TITLE_ROW, menu->title);

    for (uint8_t i = 0; i < menu->option_count; i++) {
        uint8_t row = get_row_for_menu_item_index(i);
        draw_win_text(MENU_OPTION_COL, row, menu->options[i].label);
        draw_win_text(
            MENU_CURSOR_COL,
            row,
            i == selected_index ? MENU_CURSOR_COLUMN_SELECTED : MENU_CURSOR_COLUMN_NOT_SELECTED);
    }
}

void select_menu_item(uint8_t previous_selected_index, uint8_t current_selected_index)
{
    uint8_t previous_row = get_row_for_menu_item_index(previous_selected_index);
    uint8_t current_row = get_row_for_menu_item_index(current_selected_index);

    draw_win_text(MENU_CURSOR_COL, previous_row, MENU_CURSOR_COLUMN_NOT_SELECTED);
    draw_win_text(MENU_CURSOR_COL, current_row, MENU_CURSOR_COLUMN_SELECTED);
}

void blink_menu_cursor(uint8_t selected_index, uint16_t tick)
{
    uint8_t row = get_row_for_menu_item_index(selected_index);
    bool cursor_on = tick_toggle_state(tick, MENU_CURSOR_BLINK_PERIOD_TICKS);
    draw_win_text(
        MENU_CURSOR_COL,
        row,
        cursor_on ? MENU_CURSOR_COLUMN_SELECTED : MENU_CURSOR_COLUMN_NOT_SELECTED);
}

void hide_menu(void)
{
    clear_win();
    SHOW_SPRITES;
}