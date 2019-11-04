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
static uint8_t			m_rtls_buffer[RTLS_BUFFER_SIZE];



static void beacon_timer_callback(tick_timer_t tt, void* arg)
{
	LOGI(TAG, "sending beacon...\n");

	mac_transmit_beacon();

	int maxN = RAND_MAX;
	int randN = rand();
	int nextBeaconMs = 4950 + (int)((float)randN/maxN*100);

	tick_timer_start(m_beacon_timer, nextBeaconMs, beacon_timer_callback, NULL);
}


static void mac_package_rx_callback(mac_general_package_format_t* pkg, int length, uint64_t rx_timestamp)
{
	LOGI(TAG, "rx msg (fctl: %02X, seq: %02X, S: %04X, D: %04X, size: %d)\n", pkg->fctrl, pkg->seqid, pkg->src_addr, pkg->dst_addr, length);

	if(pkg->fctrl == MAC_FRAME_TYPE_RANGING_POLL ||
			pkg->fctrl == MAC_FRAME_TYPE_RANGING_FINAL)
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
			LOGE(TAG,"RX, RTLS packet process failed: %02X", ret);
		}
	}
}

void anchor_impl_start() {
	LOGI(TAG, "Starting in anchor mode\n");

	led_blinker_init(&m_red_led, RED1_LED_PIN);
	led_blinker_init(&m_blue_led, BLUE_LED_PIN);

	led_blinker_blink(&m_red_led, 1000);

	rtls_init(addr_handler_get());

	deca_phy_init();
	mac_init(addr_handler_get(), mac_package_rx_callback);

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
