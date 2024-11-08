/**
 * @file main.c
 * @brief Főfunkciókat tartalmazó fájl
 */
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
#include "anchor_impl.h"
#include "tag_impl.h"

#define TAG "main"
//  debugging with gdb: ./JLinkGDBServer -if SWD -device nRF51822
//  ./arm-none-eabi-gdb --tui /home/strauss/munka/ble-decawave/nRF51_SDK_10.0.0_dc26b5e/examples/ble-decawave-tag/ble_app_deca/dev_deca_bt/s110/armgcc/_build/nrf51822_xxac_s110_res.out
//	(gdb) target remote localhost:2331
//	(gdb) load /home/strauss/munka/ble-decawave/nRF51_SDK_10.0.0_dc26b5e/examples/ble-decawave-tag/ble_app_deca/dev_deca_bt/s110/armgcc/_build/nrf51822_xxac_s110_res.out
//	(gdb) monitor reset/go/halt
//	(gdb) monitor memU8 <memory_address>

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

static void button_callback(button_event_t be)
{

}

int main(void)
{
	LOGI(TAG,"--> Start app <--\n");

	nrf_delay_ms(50);

	uint64_t deviceID = NRF_FICR->DEVICEID[1];
	deviceID = (deviceID << 32) & 0xFFFFFFFF00000000;
	deviceID |= NRF_FICR->DEVICEID[0] & 0xFFFFFFFF;
	LOGI(TAG,"Device id: %llu\n", deviceID);

	nrf_delay_ms(50);

	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
	NRF_CLOCK->TASKS_HFCLKSTART = 1;

	event_init();
	gpiote_init();
	button_init(button_callback);

	srand(addr_handler_get());

	if(button_read_user_button())
	{
		anchor_impl_start();
	}
	else
	{
		tag_impl_start();
	}

}


void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress )
{
	/*volatile uint32_t r0;
	volatile uint32_t r1;
	volatile uint32_t r2;
	volatile uint32_t r3;
	volatile uint32_t r12;
	volatile uint32_t lr;
	volatile uint32_t pc;
	volatile uint32_t psr;

	r0 = pulFaultStackAddress[ 0 ];
	r1 = pulFaultStackAddress[ 1 ];
	r2 = pulFaultStackAddress[ 2 ];
	r3 = pulFaultStackAddress[ 3 ];

	r12 = pulFaultStackAddress[ 4 ];
	lr = pulFaultStackAddress[ 5 ];
	pc = pulFaultStackAddress[ 6 ];
	psr = pulFaultStackAddress[ 7 ];*/

	for( ;; );
}

void HardFault_Handler( void ) __attribute__( ( naked ) );
void HardFault_Handler(void)
{
	__asm volatile
	(
		" tst lr, #4                                                \n"
		" ite eq                                                    \n"
		" mrseq r0, msp                                             \n"
		" mrsne r0, psp                                             \n"
		" ldr r1, [r0, #24]                                         \n"
		" ldr r2, handler2_address_const                            \n"
		" bx r2                                                     \n"
		" handler2_address_const: .word prvGetRegistersFromStack    \n"
	);
}

