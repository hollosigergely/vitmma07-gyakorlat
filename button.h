#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include "event.h"

typedef enum {
    BUTTON_EVENT_DOWN,
    BUTTON_EVENT_UP,
    BUTTON_EVENT_UP_LONG
} button_event_t;
typedef void (*button_callback_t)(button_event_t be);

void button_init(button_callback_t btn_callback);
void button_down_event();
bool button_tick();

#endif // BUTTON_H
