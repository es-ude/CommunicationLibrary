#include <string.h>
#include <stdlib.h>
#include "lib/include/Mac802154MRFImpl.h"
#include "test/Mocks/RuntimeLibraryImplMock.h"
#include "test/Mocks/PeripheralInterfaceMock.h"

#include "lib/src/MRFInternalConstants.h"


#define UNITY_INCLUDE_DOUBLE
#include "unity.h"
#include "CException.h"
#include "lib/include/MockPeripheral.h"
#include "lib/src/MockMRFHelperFunctions.h"
#include "test/MockMac802154MRF_TestHelper.h"


#define WRITE_BUFFER_SIZE 128
#define READ_BUFFER_SIZE 128

uint8_t write_buffer[WRITE_BUFFER_SIZE];
uint8_t read_buffer[READ_BUFFER_SIZE];

MemoryManagement *inspected_memory;
MockPeripheral mock_device = {
        .first = NULL,
        .write_buffer = write_buffer,
        .read_buffer = read_buffer,
        .write_buffer_size = WRITE_BUFFER_SIZE,
        .read_buffer_size = READ_BUFFER_SIZE,
};
Peripheral *device = (Peripheral*)&mock_device;
PeripheralInterface *interface;
Mac802154 *mrf;
Mac802154Config mrf_config;

void setUp(void) {
  memset(read_buffer, 0, READ_BUFFER_SIZE);
  memset(write_buffer, 0, WRITE_BUFFER_SIZE);
  inspected_memory = MemoryManagement_createMockImpl();
  MemoryManagement system_memory = {
          .allocate = malloc,
          .deallocate = free,
  };
  mrf_config.interface = interface;
  mrf_config.device = device;
  mrf_config.pan_id = 0;
  mrf_config.extended_source_address = 0;
  mrf_config.short_source_address = 0;
  mrf_config.channel = 11;
  mrf = Mac802154_createMRF(inspected_memory, fakeDelay);
  mock_device.current_write_buffer_position = 0;
  mock_device.current_read_buffer_position = 0;
}

void tearDown(void) {
  Mac802154_destroy(mrf);
  TEST_ASSERT_EQUAL(0, MockMemoryManagement_numberOfAllocatedObjects(inspected_memory));
}

static void setUpInitializationValues(MRF *impl, const Mac802154Config *config);

void test_initPerformsSetupLikeShownInDatasheet(void) {
  MRF *impl = (MRF *) mrf;
  setUpInitializationValues(impl, &mrf_config);
  Mac802154_init(mrf, &mrf_config);
}

void setUpInitializationValues(MRF *impl, const Mac802154Config *config) {
  MRF_setControlRegister_Expect(impl, mrf_register_software_reset, mrf_value_full_software_reset);
  MRF_setControlRegister_Expect(impl, mrf_register_power_amplifier_control2, (1 << mrf_bit_fifo_enable) | mrf_value_recommended_transmitter_on_time_before_beginning_a_packet);
  MRF_setControlRegister_Expect(impl, mrf_register_tx_stabilization, mrf_value_recommended_interframe_spacing);
  MRF_setControlRegister_Expect(impl, mrf_register_rf_control0, mrf_value_recommended_rf_optimize_control0);
  MRF_setControlRegister_Expect(impl, mrf_register_rf_control1, mrf_value_recommended_rf_optimize_control1);
  MRF_setControlRegister_Expect(impl, mrf_register_rf_control2, mrf_value_phase_locked_loop_enabled);
  MRF_setControlRegister_Expect(impl, mrf_register_rf_control6, mrf_value_enable_tx_filter | mrf_value_20MHz_clock_recovery_less_than_1ms);
  MRF_setControlRegister_Expect(impl, mrf_register_rf_control7, mrf_value_use_internal_100kHz_oscillator);
  MRF_setControlRegister_Expect(impl, mrf_register_sleep_clock_control1, mrf_value_disable_deprecated_clkout_sleep_clock_feature | mrf_value_minimum_sleep_clock_divisor_for_internal_oscillator);
  MRF_setControlRegister_Expect(impl, mrf_register_base_band2, mrf_value_clear_channel_assessment_energy_detection_only);
  MRF_setControlRegister_Expect(impl, mrf_register_energy_detection_threshold_for_clear_channel_assessment, mrf_value_recommended_energy_detection_threshold);
  MRF_setControlRegister_Expect(impl, mrf_register_base_band6, mrf_value_append_rssi_value_to_rxfifo);
  MRF_setControlRegister_Expect(impl, mrf_register_interrupt_control, (uint8_t) ~(1 << 3));

  // select channel 11, afterwards the rf state machine should be reset
  MRF_setControlRegister_Expect(impl, mrf_register_rf_control0, mrf_value_recommended_rf_optimize_control0);
  MRF_setControlRegister_Expect(impl, mrf_register_rf_mode_control, mrf_value_rf_state_machine_reset_state);
  MRF_setControlRegister_Expect(impl, mrf_register_rf_mode_control, mrf_value_rf_state_machine_operating_state);
  fakeDelay_Expect(mrf_value_delay_interval_after_state_machine_reset);

  // set transmitter power to -30dB
  MRF_setControlRegister_Expect(impl, mrf_register_rf_control3, mrf_value_transmitter_power_minus30dB);
}