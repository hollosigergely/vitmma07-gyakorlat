set(CMAKE_C_COMPILER_WORKS 1)
project(uwb_gyakorlat C ASM)
cmake_minimum_required(VERSION 2.8)

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR arm)

set(SDK_ROOT "/home/hollosi/huge/toolchain/nRF5/nRF5_SDK_15.0.0_a53641a/")
SET(CMAKE_C_COMPILER   /home/hollosi/huge/toolchain/nRF5/root/bin/arm-none-eabi-gcc)
SET(CMAKE_ASM_COMPILER /home/hollosi/huge/toolchain/nRF5/root/bin/arm-none-eabi-gcc)


SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    -DBOARD_PCA10040 \
    -DBSP_DEFINES_ONLY \
    -DCONFIG_GPIO_AS_PINRESET \
    -DFLOAT_ABI_HARD \
    -DNRF52 \
    -DNRF52832_XXAA \
    -DNRF52_PAN_74 \
    -mcpu=cortex-m4 \
    -mthumb -mabi=aapcs \
    -Wall -Werror \
    -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
    -ffunction-sections -fdata-sections -fno-strict-aliasing \
    -fno-builtin -fshort-enums")
SET(CMAKE_EXE_LINKER_FLAGS "-T${PROJECT_SOURCE_DIR}/pca10040/blank/armgcc/uart_gcc_nrf52.ld -L${SDK_ROOT}/modules/nrfx/mdk")
#SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)


set(SRCS
      ${SDK_ROOT}/modules/nrfx/mdk/gcc_startup_nrf52.S
      ${PROJECT_SOURCE_DIR}/log.c
      ${PROJECT_SOURCE_DIR}/event.c
      ${PROJECT_SOURCE_DIR}/tag_impl.c
      ${PROJECT_SOURCE_DIR}/anchor_impl.c
      ${PROJECT_SOURCE_DIR}/address_handler.c
      ${PROJECT_SOURCE_DIR}/button.c
      ${PROJECT_SOURCE_DIR}/led_blinker.c
      ${PROJECT_SOURCE_DIR}/tick_timer.c
      ${PROJECT_SOURCE_DIR}/main.c
      ${PROJECT_SOURCE_DIR}/nrf_deca.c
      ${PROJECT_SOURCE_DIR}/decadriver/deca_device.c
      ${PROJECT_SOURCE_DIR}/decadriver/deca_params_init.c
      ${PROJECT_SOURCE_DIR}/anchor/rtls.c
      ${PROJECT_SOURCE_DIR}/anchor/comm_mac.c
      ${PROJECT_SOURCE_DIR}/anchor_db.c
      ${SDK_ROOT}/components/libraries/timer/app_timer.c
      ${SDK_ROOT}/components/boards/boards.c
      ${SDK_ROOT}/components/libraries/util/app_error.c
      ${SDK_ROOT}/components/libraries/util/app_error_handler_gcc.c
      ${SDK_ROOT}/components/libraries/util/app_error_weak.c
      ${SDK_ROOT}/components/libraries/fifo/app_fifo.c
      ${SDK_ROOT}/components/libraries/uart/app_uart_fifo.c
      ${SDK_ROOT}/components/libraries/util/app_util_platform.c
      ${SDK_ROOT}/components/libraries/util/nrf_assert.c
      ${SDK_ROOT}/components/libraries/gpiote/app_gpiote.c
      ${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_gpiote.c
      ${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_twi.c
      ${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_spi.c
      ${SDK_ROOT}/components/libraries/strerror/nrf_strerror.c
      ${SDK_ROOT}/components/libraries/uart/retarget.c
      ${SDK_ROOT}/integration/nrfx/legacy/nrf_drv_uart.c
      ${SDK_ROOT}/integration/nrfx/legacy/nrf_drv_spi.c
      ${SDK_ROOT}/integration/nrfx/legacy/nrf_drv_twi.c
      ${SDK_ROOT}/modules/nrfx/drivers/src/prs/nrfx_prs.c
      ${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_uart.c
      ${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_uarte.c
      ${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_timer.c
      ${SDK_ROOT}/modules/nrfx/mdk/system_nrf52.c
      ${SDK_ROOT}/external/segger_rtt/SEGGER_RTT.c
      ${SDK_ROOT}/components/drivers_ext/lis2dh12/lis2dh12.c
      ${SDK_ROOT}/components/libraries/twi_sensor/nrf_twi_sensor.c
      ${SDK_ROOT}/components/libraries/twi_mngr/nrf_twi_mngr.c
      ${SDK_ROOT}/components/libraries/queue/nrf_queue.c
      ${SDK_ROOT}/components/libraries/balloc/nrf_balloc.c
      ${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_saadc.c
      ${SDK_ROOT}/components/libraries/scheduler/app_scheduler.c
      ${SDK_ROOT}/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c
)

add_executable(${PROJECT_NAME}  ${SRCS})
target_link_libraries(${PROJECT_NAME} c nosys m)

include_directories(
    ${SDK_ROOT}/components
    ${SDK_ROOT}/modules/nrfx/mdk
    ${SDK_ROOT}/components/libraries/experimental_log
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/pca10040/blank/config
    ${PROJECT_SOURCE_DIR}/decadriver
    ${PROJECT_SOURCE_DIR}/anchor
    ${SDK_ROOT}/components/libraries/scheduler
    ${SDK_ROOT}/components/libraries/fifo
    ${SDK_ROOT}/components/libraries/strerror
    ${SDK_ROOT}/components/toolchain/cmsis/include
    ${SDK_ROOT}/components/libraries/util
    ${SDK_ROOT}/components/libraries/balloc
    ${SDK_ROOT}/modules/nrfx/hal
    ${SDK_ROOT}/components/libraries/bsp
    ${SDK_ROOT}/components/libraries/uart
    ${SDK_ROOT}/components/libraries/gpiote
    ${SDK_ROOT}/modules/nrfx
    ${SDK_ROOT}/components/libraries/experimental_section_vars
    ${SDK_ROOT}/integration/nrfx/legacy
    ${SDK_ROOT}/components/libraries/experimental_log/src
    ${SDK_ROOT}/components/libraries/delay
    ${SDK_ROOT}/integration/nrfx
    ${SDK_ROOT}/components/drivers_nrf/nrf_soc_nosd
    ${SDK_ROOT}/components/boards
    ${SDK_ROOT}/components/libraries/experimental_memobj
    ${SDK_ROOT}/modules/nrfx/drivers/include
    ${SDK_ROOT}/components/libraries/timer
    ${SDK_ROOT}/external/segger_rtt
    ${SDK_ROOT}/components/drivers_ext/lis2dh12
    ${SDK_ROOT}/components/libraries/twi_sensor/
    ${SDK_ROOT}/components/libraries/twi_mngr/
    ${SDK_ROOT}/components/libraries/queue/
    ${SDK_ROOT}/components/libraries/balloc/
    ${SDK_ROOT}/components/libraries/pwr_mgmt/

)

