/**
 * @file comm_mac.c
 * @brief MAC kommunikációs funkciók megvalósítása
 */
#include "comm_mac.h"
#include "log.h"
#include "rtls.h"
#include "platform.h"
#include <stdlib.h>
#include <math.h>
#include "SEGGER_RTT.h"

#define TAG     "MAC"

static uint16_t m_mac_addr;
static uint8_t m_max_seq_id;

static void mac_txcallback_impl(const dwt_cb_data_t *data);
static void mac_rxok_callback_impl(const dwt_cb_data_t *data);
static void mac_rxerr_callback_impl(const dwt_cb_data_t *data);

void mac_init(uint16_t addr)
{
	m_mac_addr = addr;
	m_max_seq_id = 0;

	dwt_setcallbacks(mac_txcallback_impl, mac_rxok_callback_impl, NULL, mac_rxerr_callback_impl);
}

uint8_t mac_generate_seq_id() {
	return m_max_seq_id++;
}

static void mac_txcallback_impl(const dwt_cb_data_t *data)
{
	dwt_forcetrxoff();
	dwt_rxreset();
	dwt_rxenable(0);
}

static void mac_rxok_callback_impl(const dwt_cb_data_t *data)
{

}

static void mac_rxerr_callback_impl(const dwt_cb_data_t *data)
{
	dwt_forcetrxoff();
	dwt_rxreset();
	dwt_rxenable(0);
}

int mac_transmit(mac_general_package_format_t* pkg, int length)
{
	dwt_forcetrxoff();
	dwt_writetxdata(length + 2, (uint8_t*)pkg, 0);
	dwt_writetxfctrl(length + 2, 0, 1);   // add CRC

	dwt_starttx(DWT_START_TX_IMMEDIATE);

	return 0;
}

void mac_transmit_beacon()
{
	mac_beacon_package_format_t pkg;
	pkg.hdr.fctrl = MAC_FRAME_TYPE_BEACON;
	pkg.hdr.dst_addr = 0xFFFF;
	pkg.hdr.src_addr = m_mac_addr;
	pkg.hdr.seqid = mac_generate_seq_id();
	mac_transmit(&pkg, sizeof(mac_beacon_package_format_t));
}
