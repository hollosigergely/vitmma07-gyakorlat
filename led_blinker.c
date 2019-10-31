#include "led_blinker.h"
#include "nrf_gpio.h"
#include "commons.h"
#include "tick_timer.h"
#include "log.h"

void led_blinker_init(led_blinker_t *l, int pin)
{
	l->pin = pin;
	l->state = LED_STATE_OFF;
	l->timer = tick_timer_create();

	nrf_gpio_cfg_output(pin);
	nrf_gpio_pin_write(pin, 1);
}

static void tick_timer_callback(tick_timer_t tt, void* arg)
{
	led_blinker_t* l = arg;
	l->state = LED_STATE_OFF;
	nrf_gpio_pin_write(l->pin, 1);

	LOGT("led","off (%d)\n", l->pin);
}

void led_blinker_blink(led_blinker_t *l, int interval)
{
	nrf_gpio_pin_write(l->pin, 0);
	l->state = LED_STATE_ON;
	tick_timer_start(l->timer, interval, tick_timer_callback, l);

	LOGT("led","on (%d)\n", l->pin);
}
