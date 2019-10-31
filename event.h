#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>
#include <stdbool.h>

enum event_type
{
    ET_TIMER0_TIMEOUT,
    ET_TIMER1_TIMEOUT,
    ET_BTN_USER,
	ET_DW1000_IRQ,
	ET_ACCEL_MEAS,
	ET_ACCEL_WAKE_UP,
	ET_ACCEL_NO_MOTION
};

struct event
{
        enum event_type type;
};



void event_init();

void event_stop_timer_0();
void event_set_timer_0(uint32_t ms_to_set);
void event_stop_timer_1();
void event_set_timer_1(uint32_t ms_to_set);
bool event_is_timer_1_enabled();

int event_add(struct event e);
int event_poll(struct event* e);

void event_loop();

#endif // EVENT_H
