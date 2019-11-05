#ifndef DW1000_H_
#define DW1000_H_

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "math.h"
#include "app_uart.h"
#include "app_error.h"
#include "app_util_platform.h"
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_drv_spi.h"
#include "nrf_soc.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_delay.h"
#include "deca_types.h"
#include "deca_device_api.h"
#include "deca_regs.h"
#include "SEGGER_RTT.h"

#include "rtls.h"
#include "comm_mac.h"


#define DW1000_RST		24
#define DW1000_IRQ		19
#define DW1000_SCK_PIN		16
#define DW1000_MISO_PIN		18
#define DW1000_MOSI_PIN		20
#define DW1000_SS_PIN		17


uint64_t dwm1000_get_system_time_u64(void);
uint64_t dwm1000_get_rx_timestamp_u64(void);

int deca_phy_init();

#endif /* DW1000_H_ */


