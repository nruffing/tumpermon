#include "tick_util.h"

int16_t compare_ticks(uint16_t a, uint16_t b)
{
    return (int16_t)(a - b);
}

bool tick_toggle_state(uint16_t tick, uint16_t period_ticks)
{
    return (tick % (period_ticks * 2)) < period_ticks;
}
