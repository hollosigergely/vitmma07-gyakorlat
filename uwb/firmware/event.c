#include "event.h"
#include "nrf_drv_timer.h"
#include "log.h"

#define TAG "event"

const static nrf_drv_timer_t m_main_timer_id_0 = NRF_DRV_TIMER_INSTANCE(0); //
const static nrf_drv_timer_t m_main_timer_id_1 = NRF_DRV_TIMER_INSTANCE(1); //

#define EVENT_QUEUE_CAPACITY	10

typedef struct
{
	struct event	event;
	uint8_t			free;
} event_wrapper_t;

static event_wrapper_t		event_queue[EVENT_QUEUE_CAPACITY];
static int					eq_front, eq_back;

static void event_timeout_handler_0(nrf_timer_event_t event_type, void* p_context);
static void event_timeout_handler_1(nrf_timer_event_t event_type, void* p_context);

void event_init()
{
	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
	NRF_CLOCK->TASKS_HFCLKSTART = 1;

	eq_back = eq_front = 0;
	for(int i = 0; i < EVENT_QUEUE_CAPACITY; i++)
	{
		event_queue[i].free = 1;
	}

	nrf_drv_timer_config_t timer_cfg_0 = NRF_DRV_TIMER_DEFAULT_CONFIG;
	nrf_drv_timer_init(&m_main_timer_id_0, &timer_cfg_0, event_timeout_handler_0);

	nrf_drv_timer_config_t timer_cfg_1 = NRF_DRV_TIMER_DEFAULT_CONFIG;
	nrf_drv_timer_init(&m_main_timer_id_1, &timer_cfg_1, event_timeout_handler_1);
}

void event_stop_timer_0()
{
	nrf_drv_timer_disable(&m_main_timer_id_0);

	LOGT(TAG,"T0 stop\n");
}

static void event_timeout_handler_0(nrf_timer_event_t event_type, void* p_context)
{
	struct event e;
	e.type = ET_TIMER0_TIMEOUT;
	event_add(e);

	event_stop_timer_0();
}

void event_set_timer_0(uint32_t ms_to_set)
{
	event_stop_timer_0();

	nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
	nrf_drv_timer_init(&m_main_timer_id_0, &timer_cfg, event_timeout_handler_0);
	uint32_t time_ticks = nrf_drv_timer_ms_to_ticks(&m_main_timer_id_0, ms_to_set);
	nrf_drv_timer_extended_compare(&m_main_timer_id_0, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);
	nrf_drv_timer_enable(&m_main_timer_id_0);

	LOGT(TAG,"T0 %ld\n", ms_to_set);
}

void event_stop_timer_1()
{
	nrf_drv_timer_disable(&m_main_timer_id_1);

	LOGT(TAG,"T1 stop\n");
}

static void event_timeout_handler_1(nrf_timer_event_t event_type, void* p_context)
{
	struct event e;
	e.type = ET_TIMER1_TIMEOUT;
	event_add(e);

	//event_stop_timer_1();
}

void event_set_timer_1(uint32_t ms_to_set)
{
	event_stop_timer_1();

	nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
	nrf_drv_timer_init(&m_main_timer_id_1, &timer_cfg, event_timeout_handler_1);
	uint32_t time_ticks = nrf_drv_timer_ms_to_ticks(&m_main_timer_id_1, ms_to_set);
	nrf_drv_timer_extended_compare(&m_main_timer_id_1, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);
	nrf_drv_timer_enable(&m_main_timer_id_1);

	LOGT(TAG,"T1 %ld\n", ms_to_set);
}

bool event_is_timer_1_enabled() {
	return nrf_drv_timer_is_enabled(&m_main_timer_id_1);
}


int event_add(struct event e)
{
	__disable_irq();

	int ret = 0;
	if(event_queue[eq_back].free)
	{
		event_queue[eq_back].event = e;
		event_queue[eq_back].free = 0;
		eq_back++;
		eq_back %= EVENT_QUEUE_CAPACITY;
	}
	else
	{
		ret = -1;
	}

	__enable_irq();

	return ret;
}

int event_poll(struct event* e)
{
	int ret;

	__disable_irq();
	if(!event_queue[eq_front].free)
	{
		*e = event_queue[eq_front].event;
		event_queue[eq_front].free = 1;
		eq_front++;
		eq_front %= EVENT_QUEUE_CAPACITY;

		ret = 1;
	}
	else
	{
		ret = 0;
	}

	__enable_irq();

	return ret;
}

