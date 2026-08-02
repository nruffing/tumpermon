#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <gb/cgb.h>

bool is_cgb(void);
void update_color_pallete(const palette_color_t *rgb_data);
void reset_color_pallete(void);

#endif
