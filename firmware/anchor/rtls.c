/**
 * @file rtls.c
 * @brief Távolságmérési funkciók megvalósítása
 */
#include "rtls.h"
#include "log.h"
#include "platform.h"
#include <math.h>
#include <inttypes.h>
#include "SEGGER_RTT.h"


int __errno;


/**
 * @brief Minimim számolás
 * @param a Egyik szám
 * @param b Másik szám
 * @return Minimum
 */
static inline uint16_t min(uint16_t a, uint16_t b)
{
    return (a<b)?(a):(b);
}

/**
 * @brief Maximum számolás
 * @param a Egyik szám
 * @param b Másik szám
 * @return Maximum
 */
static inline uint16_t max(uint16_t a, uint16_t b)
{
    return (a>b)?(a):(b);
}


#define TAG     "RTLS"

#define SPEED_OF_LIGHT					299702547
#define UUS_TO_DWT_TIME					65536ll

static uint16_t     _user_addr;
static uint16_t		m_tr_id;

/**
 * @brief Távolságmérés inicializálása
 * @param addr Eszköz címe
 */
void rtls_init(uint16_t addr)
{
    _user_addr = addr;
	m_tr_id = 0;
	LOGI(TAG,"RTLS: initialize (address: %04X)\n", addr);

	LOGI(TAG,"RTLS msg size: %d, %d, %d\n",sizeof(rtls_poll_msg_t),sizeof(rtls_resp_msg_t),sizeof(rtls_final_msg_t));
}


uint16_t rtls_calculate_distance(uint32_t Treply1, uint32_t Treply2, uint32_t Tround1, uint32_t Tround2)
{
    double Ra = Tround1 - RX_ANT_DLY - TX_ANT_DLY;
    double Rb = Tround2 - RX_ANT_DLY - TX_ANT_DLY;
    double Da = Treply2 + TX_ANT_DLY + RX_ANT_DLY;
    double Db = Treply1 + TX_ANT_DLY + RX_ANT_DLY;

    double tof_dtu = ((Ra * Rb - Da * Db) / (Ra + Rb + Da + Db));
    double tof = tof_dtu * DWT_TIME_UNITS;
    double distance = tof * SPEED_OF_LIGHT;

    double bias = 0;
    distance -= bias;
    return (uint16_t)(distance*100);
}

static rtls_res_t handle_poll_msg(rtls_struct_t* rtls)
{
    if(rtls->length != sizeof(rtls_poll_msg_t))
		return RTLS_SIZE_INCORRECT;

    uint64_t poll_rx_ts = rtls->rx_ts;
    uint32_t resp_tx_ts_32 = (poll_rx_ts + (POLL_RX_TO_RESP_TX_DLY_UUS * UUS_TO_DWT_TIME)) >> 8;
    uint64_t resp_tx_ts = (((uint64_t)(resp_tx_ts_32 & 0xFFFFFFFEUL)) << 8);

    rtls_poll_msg_t*    poll = (rtls_poll_msg_t*)rtls->msg;
    rtls_resp_msg_t*    resp = (rtls_resp_msg_t*)rtls->out;

    resp->mac_hdr.fctrl = MAC_FRAME_TYPE_RANGING_RESP;
	resp->mac_hdr.seqid = mac_generate_seq_id();
    resp->mac_hdr.src_addr = _user_addr;
    resp->mac_hdr.dst_addr = poll->mac_hdr.src_addr;
    resp->tr_id = poll->tr_id;

    resp->treply1 = (uint32_t)(resp_tx_ts - poll_rx_ts);
    resp->poll_tx_ts = (uint32_t)poll->poll_tx_ts;
    resp->resp_tx_ts = (uint32_t)(resp_tx_ts);

	resp->_poll_rx_ts_64 = poll_rx_ts;
	resp->_resp_tx_ts_64 = resp_tx_ts;

    rtls->tx_ts_32 = resp_tx_ts_32;
    rtls->tx_ts = resp_tx_ts;
    rtls->out_length = sizeof(rtls_resp_msg_t);

    return RTLS_OK;
}

static rtls_res_t handle_resp_msg(rtls_struct_t* rtls)
{
    if(rtls->length != sizeof(rtls_resp_msg_t))
		return RTLS_SIZE_INCORRECT;

    rtls_resp_msg_t* resp = (rtls_resp_msg_t*)rtls->msg;
    rtls_final_msg_t* final = (rtls_final_msg_t*)rtls->out;

    uint64_t resp_rx_ts = rtls->rx_ts;
    uint32_t poll_tx_ts = resp->poll_tx_ts;

    uint32_t final_tx_ts_32 = (resp_rx_ts + (RESP_RX_TO_FINAL_TX_DLY_UUS * UUS_TO_DWT_TIME)) >> 8;
    uint64_t final_tx_ts = (((uint64_t)(final_tx_ts_32 & 0xFFFFFFFEUL)) << 8);

    final->mac_hdr.fctrl = MAC_FRAME_TYPE_RANGING_FINAL;
	final->mac_hdr.seqid = mac_generate_seq_id();
    final->mac_hdr.src_addr = _user_addr;
    final->mac_hdr.dst_addr = resp->mac_hdr.src_addr;
    final->tr_id = resp->tr_id;

    final->treply1 = resp->treply1;
    final->treply2 = (uint32_t)(final_tx_ts - resp_rx_ts);
    final->tround1 = (uint32_t)resp_rx_ts - poll_tx_ts;
    final->resp_tx_ts = resp->resp_tx_ts;

    rtls->tx_ts_32 = final_tx_ts_32;
    rtls->tx_ts = final_tx_ts;
    rtls->out_length = sizeof(rtls_final_msg_t);

    return RTLS_OK;
}

static rtls_res_t handle_final_msg(rtls_struct_t* rtls)
{
    if(rtls->length != sizeof(rtls_final_msg_t))
		return RTLS_SIZE_INCORRECT;

    uint64_t    final_rx_ts = rtls->rx_ts;

    rtls_final_msg_t* final = (rtls_final_msg_t*)rtls->msg;
	rtls_dist_msg_t* dist = (rtls_dist_msg_t*)rtls->out;

	//LOGI(TAG,"t: %d %d %d %d\n", final->treply1, final->treply2, final->tround1, (uint32_t)(final_rx_ts - final->resp_tx_ts));
    uint16_t dist_cm = rtls_calculate_distance(final->treply1, final->treply2, final->tround1, (uint32_t)final_rx_ts - final->resp_tx_ts);

	dist->mac_hdr.fctrl = MAC_FRAME_TYPE_RANGING_DIST;
	dist->mac_hdr.seqid = mac_generate_seq_id();
    dist->mac_hdr.src_addr = _user_addr;
    dist->mac_hdr.dst_addr = final->mac_hdr.src_addr;
    dist->tr_id = final->tr_id;
	dist->dist_cm = dist_cm;

	dist->_final_rx_ts_64 = final_rx_ts;

    rtls->out_length = sizeof(rtls_dist_msg_t);
    rtls->tx_ts = 0;
    rtls->tx_ts_32 = 0;

	LOGI(TAG,"Sending distance: %" PRIu16 "\n", dist_cm);

    return RTLS_OK;
}

rtls_res_t rtls_compose_poll_msg(uint16_t addr, rtls_struct_t *rtls)
{
    uint32_t poll_tx_ts_32 = (rtls->rx_ts + (POLL_TX_DLY_UUS * UUS_TO_DWT_TIME)) >> 8;
    uint64_t poll_tx_ts = (((uint64_t)(poll_tx_ts_32 & 0xFFFFFFFEUL)) << 8);

    rtls_poll_msg_t* poll = (rtls_poll_msg_t*)rtls->out;
    poll->mac_hdr.fctrl = MAC_FRAME_TYPE_RANGING_POLL;
	poll->mac_hdr.seqid = mac_generate_seq_id();
    poll->mac_hdr.src_addr = _user_addr;
    poll->mac_hdr.dst_addr = addr;
    poll->poll_tx_ts = poll_tx_ts;
	poll->tr_id = m_tr_id++;

    rtls->tx_ts = poll_tx_ts;
    rtls->tx_ts_32 = poll_tx_ts_32;
    rtls->out_length = sizeof(rtls_poll_msg_t);

    return RTLS_OK;
}

rtls_res_t rtls_handle_message(rtls_struct_t* rtls, uint8_t fctrl)
{
    if(rtls->length < 6)
        return RTLS_WRONG_MSG;

	uint8_t subtype = fctrl;

    if(subtype == MAC_FRAME_TYPE_RANGING_POLL)
    {
        return handle_poll_msg(rtls);
    }
    else if(subtype == MAC_FRAME_TYPE_RANGING_RESP)
    {
        return handle_resp_msg(rtls);
    }
    else if(subtype == MAC_FRAME_TYPE_RANGING_FINAL)
    {
        return handle_final_msg(rtls);
    }

    return RTLS_UNKNOW_SUBTYPE;
}





