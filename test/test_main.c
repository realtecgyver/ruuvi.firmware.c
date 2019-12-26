#include "unity.h"

#include "main.h"

#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_endpoints.h"
#include "mock_ruuvi_driver_error.h"
#include "mock_ruuvi_interface_communication_radio.h"
#include "mock_ruuvi_interface_log.h"
#include "mock_ruuvi_interface_rtc.h"
#include "mock_ruuvi_interface_scheduler.h"
#include "mock_ruuvi_interface_watchdog.h"
#include "mock_ruuvi_interface_yield.h"
#include "mock_task_acceleration.h"
#include "mock_task_adc.h"
#include "mock_task_advertisement.h"
#include "mock_task_button.h"
#include "mock_task_communication.h"
#include "mock_task_environmental.h"
#include "mock_task_flash.h"
#include "mock_task_gatt.h"
#include "mock_task_gpio.h"
#include "mock_task_led.h"
#include "mock_task_i2c.h"
#include "mock_task_nfc.h"
#include "mock_task_power.h"
#include "mock_task_rtc.h"
#include "mock_task_scheduler.h"
#include "mock_task_sensor.h"
#include "mock_task_spi.h"
#include "mock_task_timer.h"
#include "mock_test_sensor.h"
#include "mock_test_acceleration.h"
#include "mock_test_adc.h"
#include "mock_test_environmental.h"
#include "mock_test_library.h"

void setUp (void)
{
    ruuvi_driver_error_check_Ignore();
    ruuvi_interface_log_Ignore();
}

void tearDown (void)
{
    task_adc_vdd_sample_IgnoreAndReturn (RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_rtc_millis_IgnoreAndReturn (0);
    on_radio (RUUVI_INTERFACE_COMMUNICATION_RADIO_AFTER);
}

void test_main_NeedToImplement (void)
{
    TEST_IGNORE_MESSAGE ("Need to Implement main");
}

/**
 *  @brief Synchronize ADC measurement to radio.
 *  This is common to all radio modules, i.e.
 *  the callback gets called for every radio action.
 *
 *  @param[in] evt Type of radio event
 */
void test_main_on_radio_prepare_update (void)
{
    ruuvi_interface_rtc_millis_ExpectAndReturn (APPLICATION_ADC_SAMPLE_INTERVAL_MS * 2U);
    task_adc_vdd_prepare_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    on_radio (RUUVI_INTERFACE_COMMUNICATION_RADIO_BEFORE);
}

void test_main_on_radio_no_need_to_update (void)
{
    ruuvi_interface_rtc_millis_ExpectAndReturn (APPLICATION_ADC_SAMPLE_INTERVAL_MS / 2U);
    on_radio (RUUVI_INTERFACE_COMMUNICATION_RADIO_BEFORE);
    ruuvi_interface_rtc_millis_ExpectAndReturn (APPLICATION_ADC_SAMPLE_INTERVAL_MS / 2U);
    on_radio (RUUVI_INTERFACE_COMMUNICATION_RADIO_AFTER);
}

void test_main_on_radio_do_update (void)
{
    test_main_on_radio_prepare_update();
    task_adc_vdd_sample_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_rtc_millis_ExpectAndReturn (APPLICATION_ADC_SAMPLE_INTERVAL_MS * 2U);
    on_radio (RUUVI_INTERFACE_COMMUNICATION_RADIO_AFTER);
}

/** @brief Handle NUS connection event.
 *
 * Configures application to start sending the data via GATT instead of
 * BLE advertisements.
 *
 * @param data Unused, contains event data which is NULL.
 * @param data_len Unused, always 0.
 */
void test_main_on_gatt_connected_isr (void)
{
    task_advertisement_stop_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    task_communication_heartbeat_configure_ExpectAnyArgsAndReturn (RUUVI_DRIVER_SUCCESS);
    on_gatt_connected_isr (NULL, 0);
}

/** @brief Handle NUS disconnection event.
 *
 * Configures application to start data in  BLE advertisements.
 *
 * @param data Unused, contains event data which is NULL.
 * @param data_len Unused, always 0.
 */
void test_main_on_gatt_disconnected_isr (void * data, size_t data_len)
{
    task_communication_heartbeat_configure_ExpectAnyArgsAndReturn (RUUVI_DRIVER_SUCCESS);
    task_advertisement_start_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    on_gatt_disconnected_isr (NULL, 0);
}

/** @brief Handle incoming NUS data outside interrupt context.
 *
 * Pauses heartbeat transmissions and passes the data along with
 * reply function pointer to task communication.
 *
 * @param data Unused, contains event data which is NULL.
 * @param data_len Unused, always 0.
 */
void test_main_process_gatt_command_ok (void)
{
    ruuvi_interface_communication_message_t msg = {0};
    char data[] = "ABCDEFGHIJ";
    snprintf (& (msg.data), sizeof (data), data);
    msg.data_length = sizeof (data);
    task_communication_heartbeat_configure_ExpectAnyArgsAndReturn (RUUVI_DRIVER_SUCCESS);
    task_communication_on_data_ExpectAndReturn (&msg, &task_gatt_send_asynchronous,
            RUUVI_DRIVER_SUCCESS);
    task_communication_heartbeat_configure_ExpectAnyArgsAndReturn (RUUVI_DRIVER_SUCCESS);
    process_gatt_command (data, sizeof (data));
}

void test_main_process_gatt_command_null (void)
{
    process_gatt_command (NULL, 0);
}

void test_main_process_gatt_command_toolong (void)
{
    char data[] = "ABCDEFGHIJ";
    process_gatt_command (data, (RUUVI_INTERFACE_COMMUNICATION_MESSAGE_MAX_LENGTH + 1));
}

/** @brief Handle incoming NUS data inside interrupt context.
 *
 * Schedules the processing of data to be executed outside of interrupt context.
 *
 * @param data Incoming bytes.
 * @param data_len Length of incoming bytes.
 */
void test_main_on_gatt_received_isr (void)
{
    char data[] = "ABCDEFGHIJ";
    ruuvi_interface_scheduler_event_put_ExpectAndReturn (data,  sizeof (data),
            &process_gatt_command, RUUVI_DRIVER_SUCCESS);
    on_gatt_received_isr (data, sizeof (data));
}

/** @brief Handle outgoing NUS data event.
 *
 * This function gets called once data queued to NUS is actually sent.
 *
 * @param data Unused, contains event data which is NULL.
 * @param data_len Unused, always 0.
 */
void test_main_on_gatt_sent_isr (void * data, size_t data_len)
{
    // No implementation needed.
}