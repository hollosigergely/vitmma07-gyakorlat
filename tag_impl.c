#include "tag_impl.h"
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
#include "anchor_db.h"

#define TAG "tag"
#define RED1_LED_PIN	14
#define BLUE_LED_PIN	31

static led_blinker_t	m_red_led, m_blue_led;
static uint8_t			m_rtls_buffer[RTLS_BUFFER_SIZE];
static tick_timer_t		m_poll_timer;
static int				m_poll_anchor_index = 0;

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

static void mac_package_rx_callback(mac_general_package_format_t* pkg, int length, uint64_t rx_timestamp)
{
	LOGI(TAG, "rx msg (fctl: %02X, seq: %02X, S: %04X, D: %04X, size: %d)\n", pkg->fctrl, pkg->seqid, pkg->src_addr, pkg->dst_addr, length);

	if(pkg->fctrl == MAC_FRAME_TYPE_BEACON)
	{
		if(anchor_db_add(pkg->src_addr))
		{
			LOGI(TAG, "new anchor saved: %02X\n", pkg->src_addr);
		}
	}
	else if((pkg->fctrl & 0xF0) == MAC_FRAME_TYPE_RANGING)
	{
		if(pkg->fctrl == MAC_FRAME_TYPE_RANGING_DIST)
		{
			rtls_dist_msg_t* msg = (rtls_dist_msg_t*)pkg;
			LOGI(TAG,"distance: %d\n", msg->dist_cm);
		}
		else
		{
			rtls_struct_t rtls;
			rtls.rx_ts = rx_timestamp;
			rtls.msg = pkg;
			rtls.length = length;
			rtls.out = m_rtls_buffer;

			rtls_res_t ret = rtls_handle_message(&rtls, pkg->fctrl);
			if(ret == RTLS_OK)
			{
				mac_transmit_delayed(rtls.out, rtls.out_length, rtls.tx_ts_32);
			}
			else
			{
				LOGE(TAG,"RX, RTLS packet process failed: %02X\n", ret);
			}
		}
	}
}

static void poll_timer_callback(tick_timer_t tt, void* arg)
{
	if(anchor_db_get_size() > 0)
	{
		uint16_t anchor_id = anchor_db_get_anchor(m_poll_anchor_index);

		rtls_struct_t rtls;
		rtls.rx_ts = dwm1000_get_system_time_u64();
		rtls.out = m_rtls_buffer;
		rtls_compose_poll_msg(anchor_id,&rtls);
		mac_transmit_delayed(rtls.out, rtls.out_length,rtls.tx_ts_32);

		m_poll_anchor_index++;
		if(m_poll_anchor_index >= anchor_db_get_size())
			m_poll_anchor_index = 0;
	}

	tick_timer_start(m_poll_timer, 500, poll_timer_callback, NULL);
}

void tag_impl_start() {
	LOGI(TAG, "Starting in tag mode\n");

	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
	NRF_CLOCK->TASKS_HFCLKSTART = 1;

	event_init();

	led_blinker_init(&m_red_led, RED1_LED_PIN);
	led_blinker_init(&m_blue_led, BLUE_LED_PIN);

	gpiote_init();
	button_init(user_btn_callback);

	led_blinker_blink(&m_red_led, 500);
	led_blinker_blink(&m_blue_led, 500);

	anchor_db_init();
	rtls_init(addr_handler_get());

	deca_phy_init();
	mac_init(addr_handler_get(), mac_package_rx_callback);

	m_poll_timer = tick_timer_create();
	tick_timer_start(m_poll_timer, 500, poll_timer_callback, NULL);

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
