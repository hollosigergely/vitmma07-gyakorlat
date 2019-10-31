#ifndef LED_BLINKER_H
#define LED_BLINKER_H

#include <stdbool.h>
#include "tick_timer.h"

typedef enum
{
        LED_STATE_ON,
        LED_STATE_OFF
} led_blinker_state;

typedef struct
{
        int pin;
		tick_timer_t	timer;
        led_blinker_state state;
} led_blinker_t;

void led_blinker_init(led_blinker_t* l, int pin);
void led_blinker_blink(led_blinker_t* l, int interval);


#endif // LED_BLINKER_H
