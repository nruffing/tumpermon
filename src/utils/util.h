#ifndef UTIL_H
#define UTIL_H

#include <gb/cgb.h>
#include <stdbool.h>

bool is_cgb(void);
void update_color_pallete(const palette_color_t *rgb_data);
void reset_color_pallete(void);
void update_sprite_pallete(const palette_color_t *rgb_data);
void reset_screen(void);

#endif
