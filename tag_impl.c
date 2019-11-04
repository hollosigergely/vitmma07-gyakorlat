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

			LOGTS(msg->mac_hdr.src_addr, msg->tr_id, 5, msg->_final_rx_ts_64);
		}
		else if(pkg->fctrl == MAC_FRAME_TYPE_RANGING_RESP)
		{
			rtls_struct_t rtls;
			rtls.rx_ts = rx_timestamp;
			rtls.msg = (uint8_t*)pkg;
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

			rtls_resp_msg_t* msg = (rtls_resp_msg_t*)pkg;
			LOGTS(msg->mac_hdr.src_addr, msg->tr_id, 1, msg->_poll_rx_ts_64);
			LOGTS(msg->mac_hdr.src_addr, msg->tr_id, 2, msg->_resp_tx_ts_64);
			LOGTS(msg->mac_hdr.src_addr, msg->tr_id, 3, rx_timestamp);
			LOGTS(msg->mac_hdr.src_addr, msg->tr_id, 4, rtls.tx_ts);
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

		rtls_poll_msg_t* msg = (rtls_poll_msg_t*)rtls.out;
		LOGTS(anchor_id, msg->tr_id, 0, msg->poll_tx_ts);

		m_poll_anchor_index++;
		if(m_poll_anchor_index >= anchor_db_get_size())
			m_poll_anchor_index = 0;
	}

	tick_timer_start(m_poll_timer, 500, poll_timer_callback, NULL);
}

void tag_impl_start() {
	LOGI(TAG, "Starting in tag mode\n");

	led_blinker_init(&m_red_led, RED1_LED_PIN);
	led_blinker_init(&m_blue_led, BLUE_LED_PIN);

	led_blinker_blink(&m_blue_led, 1000);

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
