#ifndef MENUS_H
#define MENUS_H

#include "menu.h"

typedef enum MenuId {
    PAUSE_MENU_ID,
} MenuId;

typedef enum PauseMenuOptionId {
    PAUSE_MENU_RESUME_ID,
    PAUSE_MENU_RESTART_ID,
} PauseMenuOptionId;

static const MenuOption pause_menu_options[] = {
    { .id = PAUSE_MENU_RESUME_ID, .label = "Resume" },
    { .id = PAUSE_MENU_RESTART_ID, .label = "Restart" },
};

static const Menu pause_menu = {
    .id = PAUSE_MENU_ID,
    .title = "PAUSED",
    .options = pause_menu_options,
    .option_count = 2,
};

#endif