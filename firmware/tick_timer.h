#ifndef TICK_TIMER_H
#define TICK_TIMER_H

#include <stdbool.h>

typedef int tick_timer_t;
typedef void (*tick_timer_callback_t)(tick_timer_t tt, void*);


tick_timer_t tick_timer_create();
void tick_timer_start(tick_timer_t tt, int timeout, tick_timer_callback_t timeoutcb, void* arg);
void tick_timer_stop(tick_timer_t tt);

bool tick_timers_tick();
void tick_timers_enabled(bool enabled);

bool tick_timer_is_running(tick_timer_t tt);
bool tick_timer_is_valid(tick_timer_t tt);
#endif // TICK_TIMER_H
