#ifndef APA_UTIL_H
#define APA_UTIL_H

#include <stdint.h>

void do_apa_mode(void);
void end_apa_mode(void);
uint8_t get_centered_x(const char *str);
void print_centered(const char *str, uint8_t y);

#endif
