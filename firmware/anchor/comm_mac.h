/**
 * @file comm_mac.h
 * @brief A MAC kommunikációs réteg header fájla
 */
#ifndef __MAC_H_
#define __MAC_H_

#include <stdint.h>
#include "decadriver/deca_device_api.h"
#include "decadriver/deca_regs.h"
#include "misc.h"
#include "stdbool.h"

#define MAC_FRAME_TYPE_CONTROL			0x00
#define MAC_FRAME_TYPE_BEACON			0x01

#define MAC_FRAME_TYPE_RANGING			0x10
#define MAC_FRAME_TYPE_RANGING_POLL     0x10
#define MAC_FRAME_TYPE_RANGING_RESP     0x11
#define MAC_FRAME_TYPE_RANGING_FINAL    0x12
#define MAC_FRAME_TYPE_RANGING_DIST     0x13

typedef struct {
	uint8_t     fctrl;
	uint8_t     seqid;
	uint16_t    src_addr;
	uint16_t    dst_addr;
	uint16_t    padding;
	uint8_t     payload[0];
} __packed mac_general_package_format_t;

typedef struct {
	mac_general_package_format_t	hdr;
} __packed mac_beacon_package_format_t;


typedef void (*mac_package_rx_callback_t)(mac_general_package_format_t* pkg, int length, uint64_t rx_timestamp);

void		mac_init(uint16_t addr, mac_package_rx_callback_t callback);
uint8_t		mac_generate_seq_id();

int			mac_transmit(void* pkg, int length);
int			mac_transmit_delayed(void* pkg, int length, uint32_t txts32);
void		mac_transmit_beacon();

#endif
