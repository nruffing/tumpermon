#ifndef MENU_H
#define MENU_H

#include <stdint.h>

typedef struct MenuOption {
    uint8_t id;
    const char *label;
} MenuOption;

typedef struct Menu {
    uint8_t id;
    const char *title;
    const MenuOption *options;
    uint8_t option_count;
} Menu;

typedef struct MenuContext {
    const Menu *open_menu;
    uint8_t selected_index;
} MenuContext;

void show_menu(const Menu *menu, uint8_t selected_index);
void hide_menu(void);
void select_menu_item(uint8_t previous_selected_index, uint8_t current_selected_index);
void blink_menu_cursor(uint8_t selected_index, uint16_t tick);

#endif