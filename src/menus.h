#ifndef MENUS_H
#define MENUS_H

#include "menu.h"

static const MenuOption pause_menu_options[] = {
    { .label = "Restart" },
    { .label = "Resume" },
};

static const Menu pause_menu = {
    .title = "PAUSED",
    .options = pause_menu_options,
    .option_count = 2,
};

#endif