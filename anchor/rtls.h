/**
 * @file rtls.h
 * @brief Helymeghatározási funkció header fájla
 */
#ifndef __COMM_RTLS_H_
#define __COMM_RTLS_H_

#include <stdint.h>
#include <string.h>
#include "comm_mac.h"
#include "commons.h"
#include "assert.h"

#define RTLS_BUFFER_SIZE    128

typedef uint8_t rtls_res_t;
#define RTLS_OK             0
#define RTLS_ERR            1
#define RTLS_WRONG_MSG      2
#define RTLS_UNKNOW_SUBTYPE 3
#define RTLS_IN_PROGRESS    4
#define RTLS_SIZE_INCORRECT 5

typedef struct
{
	// Input
    uint64_t        rx_ts;
    uint8_t*        msg;
    uint16_t        length;

	// Output
    uint8_t*        out;
    uint16_t        out_length;
    uint32_t        tx_ts_32;
    uint64_t        tx_ts;
} rtls_struct_t;

#define RX_QUALITY_FLAG_NLOS    0x01
#define RX_QUALITY_FLAG_ERR     0x02


typedef struct
{
    mac_general_package_format_t    mac_hdr;
    uint64_t                        poll_tx_ts;
    uint8_t                         tr_id;
} __packed rtls_poll_msg_t;

typedef struct
{
    mac_general_package_format_t    mac_hdr;

	// Misc
	uint64_t						_poll_rx_ts_64;
	uint64_t						_resp_tx_ts_64;
	// !Misc

    uint32_t                        treply1;
    uint32_t                        poll_tx_ts;
    uint32_t                        resp_tx_ts;
    uint8_t                         tr_id;
} __packed rtls_resp_msg_t;

typedef struct
{
    mac_general_package_format_t    mac_hdr;
    uint32_t                        treply1;
    uint32_t                        treply2;
    uint32_t                        tround1;
    uint32_t                        resp_tx_ts;
    uint8_t                         tr_id;
} __packed rtls_final_msg_t;

typedef struct
{
	mac_general_package_format_t    mac_hdr;

	// Misc
	uint64_t						_final_rx_ts_64;
	// !Misc

	uint16_t						dist_cm;
	uint8_t                         tr_id;
} __packed rtls_dist_msg_t;

void        rtls_init(uint16_t addr);

rtls_res_t  rtls_handle_message(rtls_struct_t* rtls, uint8_t fctrl);
rtls_res_t  rtls_compose_poll_msg(uint16_t addr, rtls_struct_t* rtls);

uint16_t    rtls_calculate_distance(uint32_t Treply1, uint32_t Treply2, uint32_t Tround1, uint32_t Tround2);

#endif
