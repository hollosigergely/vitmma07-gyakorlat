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
#include "nrf_deca.h"
#include "SEGGER_RTT.h"

#define TAG     "MAC"

static uint16_t m_mac_addr;
static uint8_t m_max_seq_id;
static uint8_t m_buffer[RTLS_BUFFER_SIZE];
static mac_package_rx_callback_t m_callback = NULL;

static void mac_txcallback_impl(const dwt_cb_data_t *data);
static void mac_rxok_callback_impl(const dwt_cb_data_t *data);
static void mac_rxerr_callback_impl(const dwt_cb_data_t *data);

void mac_init(uint16_t addr, mac_package_rx_callback_t callback)
{
	LOGI(TAG, "Init MAC: %04X\n", addr);

	m_mac_addr = addr;
	m_max_seq_id = 0;
	m_callback = callback;

	dwt_setcallbacks(mac_txcallback_impl, mac_rxok_callback_impl, NULL, mac_rxerr_callback_impl);

	dwt_rxenable(0);
}

uint8_t mac_generate_seq_id() {
	return m_max_seq_id++;
}

static void mac_txcallback_impl(const dwt_cb_data_t *data)
{
	LOGT(TAG, "TX callback\n");

	dwt_forcetrxoff();
	dwt_rxreset();
	dwt_rxenable(0);
}

static void mac_rxok_callback_impl(const dwt_cb_data_t *data)
{
	LOGT(TAG, "RX\n");

	if(data->datalength - 2 < sizeof(mac_general_package_format_t))
	{
		LOGE(TAG, "small package, drop\n");

		dwt_rxenable(0);
		return;
	}

	dwt_readrxdata(m_buffer, data->datalength - 2, 0);
	uint64_t rxts = dwm1000_get_rx_timestamp_u64();

	mac_general_package_format_t* pkg = (mac_general_package_format_t*)m_buffer;
	if(pkg->dst_addr != 0xFFFF &&
			pkg->dst_addr != m_mac_addr)
	{
		LOGT(TAG, "Wrong address, drop\n");

		dwt_rxenable(0);
		return;
	}

	if(m_callback != NULL)
		m_callback((mac_general_package_format_t*)m_buffer, data->datalength - 2, rxts);

	dwt_rxenable(0);
}

static void mac_rxerr_callback_impl(const dwt_cb_data_t *data)
{
	LOGT(TAG, "RX Error\n");

	dwt_forcetrxoff();
	dwt_rxreset();
	dwt_rxenable(0);
}

int mac_transmit(void* pkg, int length)
{
	dwt_forcetrxoff();
	dwt_writetxdata(length + 2, (uint8_t*)pkg, 0);
	dwt_writetxfctrl(length + 2, 0, 1);   // add CRC

	dwt_starttx(DWT_START_TX_IMMEDIATE);

	return 0;
}

int mac_transmit_delayed(void* pkg, int length, uint32_t txts32)
{
	if(txts32 == 0)
		return mac_transmit(pkg,length);

	dwt_forcetrxoff();
	dwt_writetxdata(length + 2, (uint8_t*)pkg, 0);
	dwt_writetxfctrl(length + 2, 0, 1);   // add CRC

	dwt_setdelayedtrxtime(txts32);
	dwt_starttx(DWT_START_TX_DELAYED);

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
