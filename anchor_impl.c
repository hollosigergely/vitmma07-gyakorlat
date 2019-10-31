#include "anchor_impl.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "board.h"
#include "nrf_gpio.h"
#include "app_gpiote.h"
#include "nrf_drv_gpiote.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_saadc.h"
#include "nrf_deca.h"
#include "nrf_drv_uart.h"
#include "app_uart.h"
#include "nrf_drv_twi.h"
#include "commons.h"
#include "SEGGER_RTT.h"
#include "log.h"
#include "address_handler.h"
#include "event.h"
#include "led_blinker.h"
#include "button.h"



#define RED1_LED_PIN	14
#define BLUE_LED_PIN	31
static led_blinker_t m_red_led, m_blue_led;
static tick_timer_t m_beacon_timer;

static void gpiote_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	if(pin == DW1000_IRQ)
	{
		LOGT(TAG,"DW1000 IRQ\n");
		struct event e = { ET_DW1000_IRQ };
		event_add(e);
	}
}

static void gpiote_init()
{
	ret_code_t err_code;

	err_code = nrf_drv_gpiote_init();
	APP_ERROR_CHECK(err_code);

	nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
	in_config.pull = NRF_GPIO_PIN_PULLDOWN;
	err_code = nrf_drv_gpiote_in_init(DW1000_IRQ, &in_config, gpiote_event_handler);
	APP_ERROR_CHECK(err_code);

	nrf_drv_gpiote_in_event_enable(DW1000_IRQ, true);
}

static void user_btn_callback(button_event_t be)
{
	switch(be)
	{
	case BUTTON_EVENT_DOWN:
		break;
	case BUTTON_EVENT_UP:
		//tick_timer_start(m_push_button_timer, 3000, tick_timer_callback, NULL);
		break;
	case BUTTON_EVENT_UP_LONG:
		/*if(m_tag_running_state == TAG_RUNNING_STATE_OFF)
		{
			set_tag_state(TAG_RUNNING_STATE_RUNNING);
		}
		else if(m_tag_running_state == TAG_RUNNING_STATE_RUNNING)
		{
			set_tag_state(TAG_RUNNING_STATE_GOING_OFF);
		}*/
		break;
	}
}

static void beacon_timer_callback(tick_timer_t tt, void* arg)
{
	LOGI(TAG, "sending beacon...\n");

	mac_transmit_beacon();

	tick_timer_start(m_beacon_timer, 1000, beacon_timer_callback, NULL);
}



void anchor_impl_initialize()
{

}

void anchor_impl_start() {
	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
	NRF_CLOCK->TASKS_HFCLKSTART = 1;

	event_init();

	led_blinker_init(&m_red_led, RED1_LED_PIN);
	led_blinker_init(&m_blue_led, BLUE_LED_PIN);

	gpiote_init();
	button_init(user_btn_callback);

	led_blinker_blink(&m_red_led, 500);
	led_blinker_blink(&m_blue_led, 500);

	deca_phy_init();
	mac_init(0x8001, NULL);

	m_beacon_timer = tick_timer_create();
	tick_timer_start(m_beacon_timer, 1000, beacon_timer_callback, NULL);

	for (;;)
	{
		struct event event;
		while(event_poll(&event))
		{
			if(event.type == ET_TIMER0_TIMEOUT)
			{
				LOGT(TAG,"timer0 event\n");

			}
			else if(event.type == ET_TIMER1_TIMEOUT)
			{
				LOGT(TAG,"timer1 event\n");
				tick_timers_tick();
			}
			else if(event.type == ET_BTN_USER)
			{
				LOGT(TAG,"user btn event\n");

			}
			else if(event.type == ET_DW1000_IRQ)
			{
				LOGT(TAG,"dw_irq event\n");
				while(nrf_gpio_pin_read(DW1000_IRQ) == 1)
				{
					dwt_isr();
				}
			}
		}

		__SEV();
		__WFE();
		__WFE();
		LOGT(TAG,"--- wake up\n");
	}
}
