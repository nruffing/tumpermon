#include "menu.h"

#include <gb/gb.h>

#include "window.h"

// WX (the window's X register, set via move_win's x arg) is offset from
// screen space by hardware design: WX=7 puts the window's left edge at
// screen pixel 0. WX 0-6 shifts the window partially off-screen (and WX=0
// is glitchy on real DMG hardware), so this offset can't be zero.
#define WX_SCREEN_ORIGIN_OFFSET 7

static uint8_t get_win_position_x(uint8_t pixel_x)
{
    return pixel_x + WX_SCREEN_ORIGIN_OFFSET;
}

// Tile row/column the title and first option are drawn at, in window-tile
// coordinates (not screen pixels — see get_win_position_x for that offset).
#define MENU_TITLE_ROW 1
#define MENU_TITLE_COL 1
#define MENU_OPTIONS_START_ROW 3
#define MENU_OPTION_COL 2
#define MENU_CURSOR_COL (MENU_OPTION_COL - 1)

#define MENU_CURSOR_COLUMN_SELECTED ">"
#define MENU_CURSOR_COLUMN_NOT_SELECTED " "

static uint8_t get_row_for_menu_item_index(uint8_t menu_item_index)
{
    return (uint8_t)(MENU_OPTIONS_START_ROW + menu_item_index);
}

void show_menu(const Menu *menu, uint8_t selected_index)
{
    HIDE_WIN;
    HIDE_SPRITES;

    move_win(get_win_position_x(0), 0);

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

    SHOW_WIN;
}

void select_menu_item(uint8_t previous_selected_index, uint8_t current_selected_index)
{
    uint8_t previous_row = get_row_for_menu_item_index(previous_selected_index);
    uint8_t current_row = get_row_for_menu_item_index(current_selected_index);

    draw_win_text(MENU_CURSOR_COL, previous_row, MENU_CURSOR_COLUMN_NOT_SELECTED);
    draw_win_text(MENU_CURSOR_COL, current_row, MENU_CURSOR_COLUMN_SELECTED);
}

void hide_menu(void)
{
    HIDE_WIN;
    SHOW_SPRITES;
}