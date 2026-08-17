#ifndef MENU_H
#define MENU_H

#include <stdint.h>

typedef struct MenuOption {
    const char *label;
} MenuOption;

typedef struct Menu {
    const char *title;
    const MenuOption *options;
    uint8_t option_count;
} Menu;

typedef struct MenuContext {
    uint8_t pause_menu_selected_index;
} MenuContext;

void show_menu(const Menu *menu, uint8_t selected_index);
void hide_menu(void);

#endif