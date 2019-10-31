#ifndef COMMONS_H_
#define COMMONS_H_

//#define CALIB
#define CALIB_ANCHOR_ADDRESS		    0x8003

#define	SERIAL_RX_PIN			    RX_PIN_NUMBER
#define	SERIAL_TX_PIN			    TX_PIN_NUMBER

#define BLE_NUS_MAX_DATA_LEN		    100

#define IS_SRVC_CHANGED_CHARACT_PRESENT	    0						/**< Include or not the service_changed characteristic. if not enabled, the server's database cannot be changed for the lifetime of the device*/

#define HR_INC_BUTTON_PIN_NO		    BUTTON_0					/**< Button used to increment heart rate. */
#define HR_DEC_BUTTON_PIN_NO		    BUTTON_1					/**< Button used to decrement heart rate. */
#define BOND_DELETE_ALL_BUTTON_ID	    HR_DEC_BUTTON_PIN_NO			/**< Button used for deleting all bonded centrals during startup. */

#define DEVICE_NAME			    "DecaLoc"					/**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME		    "BME ETIK"					/**< Manufacturer. Will be passed to Device Information Service. */
#define APP_ADV_INTERVAL		    MSEC_TO_UNITS(250, UNIT_0_625_MS)		/**< The advertising interval (in units of 0.625 ms. This value corresponds to 25 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS	    0						/**< The advertising timeout in units of seconds. */

#define APP_TIMER_PRESCALER		    0						/**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE     10						/**< Size of timer operation queues. */

#define UART_TYPE_POLL_MSG          0x04
#define UART_TYPE_SUPERFRAME_START  0x05
#define UART_TYPE_RANGING_MSG       0x06

#define TIMER_TICK_MS						100
#define TAG_AFTER_OFF_STATUS_INTERVAL_MS	3000
#define TAG_AFTER_SLEEP_STATUS_INTERVAL_MS	2000
#define TAG_BLUE_LED_SUPERFRAME_COUNT		10
#define TAG_BATTERY_MEASUREMENT_INTERVAL_MS	5000

#define TDOA_AGGR_COUNT             56
#define TDOA_ANCHOR_COUNT           8
#define TDOA_AGGR_TIME_SLOT         25      //ms
#define TDOA_POLL_TIME_SLOT         7       //ms

#define RANGING_POLL_TIME_SLOT      35      //ms
#define RANGING_COUNT_AT_STARTUP    120

#define TDOA_POLL_TX_DELAY          5000ll  //us
#define TDOA_AGGR_TX_DELAY          6000ll  //us

#define TDOA_SUPERFRAME_INTERVAL    500     //ms
#define TDOA_ANCHOR_INTERVAL        TDOA_ANCHOR_COUNT*TDOA_AGGR_TIME_SLOT    //ms
#define TDOA_TAG_INTERVAL           TDOA_SUPERFRAME_INTERVAL-TDOA_ANCHOR_INTERVAL   //ms

#define AGGR_INDEX_START            55

#define RX_TIMEOUT                  250     //ms

#define APP_GPIOTE_MAX_USERS		    2						/**< Maximum number of users of the GPIOTE handler. */

#define BUTTON_DETECTION_DELAY		    APP_TIMER_TICKS(50)	/**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */

#define MIN_CONN_INTERVAL		    MSEC_TO_UNITS(8, UNIT_1_25_MS)		/**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL		    MSEC_TO_UNITS(12, UNIT_1_25_MS)		/**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY			    0						/**< Slave latency. */
#define CONN_SUP_TIMEOUT		    MSEC_TO_UNITS(4000, UNIT_10_MS)		/**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY	    APP_TIMER_TICKS(5000)	/**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY	    APP_TIMER_TICKS(30000)	/**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT	    3						/**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_TIMEOUT		    30						/**< Timeout for Pairing Request or Security Request (in seconds). */
#define SEC_PARAM_BOND			    1						/**< Perform bonding. */
#define SEC_PARAM_MITM			    0						/**< Man In The Middle protection not required. */
#define SEC_PARAM_IO_CAPABILITIES	    BLE_GAP_IO_CAPS_NONE			/**< No I/O capabilities. */
#define SEC_PARAM_OOB			    0						/**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE		    7						/**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE		    16						/**< Maximum encryption key size. */

#define DEAD_BEEF			    0xDEADBEEF					/**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#endif /* COMMONS_H_ */

