#include "tick_timer.h"
#include "commons.h"
#include "log.h"
#include "event.h"

#define TAG "ttimer"
#define TICK_TIMER_MAX_COUNT	10

typedef struct {
	int                     tick_count;
	int                     timeout;
	bool                    running;
	tick_timer_callback_t   timeoutcb;
	void*					arg;
} timer_struct_t;

static timer_struct_t	m_timers[TICK_TIMER_MAX_COUNT];
static unsigned			m_timer_count = 0;

static void start_tick_timer()
{
	if(!event_is_timer_1_enabled())
	{
		LOGI(TAG,"tick start\n");
		event_set_timer_1(TIMER_TICK_MS);
	}
}

static void stop_tick_timer()
{
	LOGI(TAG,"tick stop\n");
	event_stop_timer_1();
}

tick_timer_t tick_timer_create()
{
	if(m_timer_count >= TICK_TIMER_MAX_COUNT)
	{
		ERROR(TAG,"No more timer is available");
	}

	int tidx = m_timer_count;
	m_timers[tidx].tick_count = -1;
	m_timers[tidx].running = false;

	m_timer_count++;

	return tidx;
}

void tick_timer_start(tick_timer_t tt, int timeout, tick_timer_callback_t timeoutcb, void* arg)
{
	m_timers[tt].tick_count = 0;
	m_timers[tt].timeout = timeout;
	m_timers[tt].timeoutcb = timeoutcb;
	m_timers[tt].running = true;
	m_timers[tt].arg = arg;

	start_tick_timer();
}

void tick_timer_stop(tick_timer_t tt)
{
	m_timers[tt].tick_count = -1;
	m_timers[tt].running = false;

	tick_timers_tick();
}

bool tick_timers_tick()
{
	for(int i = 0; i < m_timer_count; i++)
	{
		m_timers[i].tick_count++;

		if(m_timers[i].running && m_timers[i].tick_count * TIMER_TICK_MS >= m_timers[i].timeout)
		{
			m_timers[i].running = false;
			if(m_timers[i].timeoutcb != NULL)
				m_timers[i].timeoutcb(i, m_timers[i].arg);
		}
	}

	bool needs_tick = false;
	for(int i = 0; i < m_timer_count; i++)
	{
		needs_tick |= m_timers[i].running;
	}

	if(!needs_tick)
		stop_tick_timer();

	return needs_tick;
}

void tick_timers_enabled(bool enabled)
{
	if(enabled)
	{
		start_tick_timer();
	}
	else
	{
		stop_tick_timer();
	}
}

bool tick_timer_is_running(tick_timer_t tt)
{
	return m_timers[tt].running;
}

bool tick_timer_is_valid(tick_timer_t tt)
{
	return m_timers[tt].tick_count >= 0;
}
