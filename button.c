#include "button.h"
#include "nrf_gpio.h"
#include "app_gpiote.h"
#include "nrf_drv_gpiote.h"
#include "log.h"
#include "event.h"
#include "commons.h"
#include "tick_timer.h"

#define TAG "btn"

#define BUTTON_CHECK_INTERVAL_MS	100
#define BUTTON_LONG_PRESS_MS		2000

#define BATTERY_MEAS_PIN        30
#define BATTERY_MEAS_GND_PIN    12

#define RASP_BUTTON_PIN	26
#define USER_BUTTON_PIN	2
#define BLUE_LED_PIN	31
#define RED1_LED_PIN	14

static void user_button_enable_event();
static void user_button_disable_event();

typedef enum {
	BUTTON_STATE_IDLE,
	BUTTON_STATE_DOWN,
	BUTTON_STATE_DISPATCHED
} button_state_t;

static button_callback_t m_button_callback = NULL;
static tick_timer_t		m_timer;
static intmax_t			m_btn_check_count = 0;
static button_state_t   m_btn_state = BUTTON_STATE_IDLE;


static void button_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	LOGT(TAG,"user_btn irq\n");

	struct event e = { ET_BTN_USER };
	event_add(e);

	user_button_disable_event();
}

static inline void user_button_enable_event()
{
	nrf_drv_gpiote_in_event_enable(USER_BUTTON_PIN, true);
}

static inline void user_button_disable_event()
{
	LOGT(TAG,"btn_irq disable\n");
	nrf_drv_gpiote_in_event_disable(USER_BUTTON_PIN);
}

static void tick_timer_callback(tick_timer_t tt, void* arg)
{
	uint32_t value = nrf_gpio_pin_read(USER_BUTTON_PIN);
	LOGT(TAG,"btn value: %d\n", value);

	m_btn_check_count++;

	if(value == 0)
	{
		if(m_btn_state == BUTTON_STATE_DOWN && m_btn_check_count * BUTTON_CHECK_INTERVAL_MS >= BUTTON_LONG_PRESS_MS)
		{
			LOGI(TAG,"user_btn long pressed\n");
			if(m_button_callback != NULL)
				m_button_callback(BUTTON_EVENT_UP_LONG);

			m_btn_state = BUTTON_STATE_DISPATCHED;
		}

		tick_timer_start(m_timer, BUTTON_CHECK_INTERVAL_MS, tick_timer_callback, NULL);
	}
	else
	{
		LOGT(TAG,"user_btn up\n");
		user_button_enable_event();

		if(m_btn_state == BUTTON_STATE_DOWN && m_btn_check_count * BUTTON_CHECK_INTERVAL_MS < BUTTON_LONG_PRESS_MS)
		{
			LOGI(TAG,"user_btn pressed\n");
			if(m_button_callback != NULL)
				m_button_callback(BUTTON_EVENT_UP);
		}

		m_btn_state = BUTTON_STATE_IDLE;
	}
}


void button_init(button_callback_t btn_callback)
{
	m_button_callback = btn_callback;
	m_timer = tick_timer_create();

	nrf_gpio_cfg_input(USER_BUTTON_PIN, NRF_GPIO_PIN_PULLUP);
	nrf_gpio_cfg_input(BATTERY_MEAS_PIN, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_input(BATTERY_MEAS_GND_PIN, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_input(RASP_BUTTON_PIN, NRF_GPIO_PIN_PULLUP);

	ret_code_t err_code;

	nrf_drv_gpiote_in_config_t in_config_user_btn = GPIOTE_CONFIG_IN_SENSE_HITOLO(false);
	in_config_user_btn.pull = NRF_GPIO_PIN_PULLUP;
	err_code = nrf_drv_gpiote_in_init(USER_BUTTON_PIN, &in_config_user_btn, button_event_handler);
	APP_ERROR_CHECK(err_code);

	user_button_enable_event();
}

void button_down_event()
{
	m_btn_state = BUTTON_STATE_DOWN;
	m_btn_check_count = 0;

	if(m_button_callback != NULL)
		m_button_callback(BUTTON_EVENT_DOWN);

	tick_timer_start(m_timer, BUTTON_CHECK_INTERVAL_MS, tick_timer_callback, NULL);
}

