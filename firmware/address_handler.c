#include "address_handler.h"
#include <nrf.h>
#include <inttypes.h>
#include "log.h"


volatile uint32_t m_uicr_data  __attribute__ ((section(".uicr_customer")));

//nrfjprog -f nrf52 --eraseuicr
//nrfjprog -f nrf52 --memwr 0x10001080 --val <val>


uint16_t addr_handler_get()
{
    return m_uicr_data;
}


