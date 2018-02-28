#include <string.h>
#include "lib/include/NetworkHardware.h"
#include "test/Mocks/MockSPIImpl.h"
#include "test/Mocks/MockRuntimeLibraryImpl.h"
#include "lib/src/NetworkHardwareMRFInternalConstants.h"
#include "lib/include/MRFHelperFunctions.h"
#include "lib/include/Exception.h"

#define UNITY_INCLUDE_DOUBLE
#include "src/unity.h"
#include "CException.h"
#include "lib/include/NetworkHardwareMRFImpl.h"

/**
 * TODO:
 * add proper exceptions using the cexception library
 * exceptions should be thrown in case the spi interface is busy
 */

#define MEMORY_CAPACITY 8*10
static uint8_t raw_memory[MEMORY_CAPACITY];

static MockAllocateConfig allocate_config = {.returned_address = raw_memory};
static SPIDeviceMockImpl mock_interface;
static uint8_t slave_select_line;
static SPISlave output_device = {
        .messageModule = (SPIMessageLayer*) &mock_interface,
        .slave_select_register = &slave_select_line,
        .slave_select_pin = 1,
};
static NetworkHardware *mrf;
static uint8_t buffer[128];

void setUp(void) {
  MockAllocate_configure(&allocate_config);
  SPIDeviceMockImpl_init(&mock_interface);
  mock_interface.input_buffer = NULL;
  mock_interface.output_buffer = buffer;
  mrf = NetworkHardware_createMRF(&output_device, MockAllocate_allocate, MockRuntime_delayMicroseconds);
  memset(buffer, 0, 128);
}

void test_initPerformsSoftwareReset(void) {
  uint8_t expected_buffer[3] = {
          MRF_writeShortCommand(mrf_register_software_reset),
          0x07};
  NetworkHardware_init(mrf);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_buffer, buffer, 2);
}

void test_initSetsTXStabilizationRegisterToRecommendedValue(void) {
  uint8_t expected_buffer[2] = {
          ((mrf_register_tx_stabilization << 1) & ~(1 << 7)) | 1,
          0x09,
  };
  NetworkHardware_init(mrf);
  uint8_t offset_from_software_reset = 2;
  uint8_t offset_from_power_amplifier_config = 2;
  uint8_t total_offset = offset_from_power_amplifier_config +
          offset_from_software_reset;
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_buffer, buffer+total_offset, 2);
}

void test_initConfiguresPowerAmplifierCorrectly(void) {
  // we just use the values from the initialization example
  // given in the datasheet here
  uint8_t command = MRF_writeShortCommand(mrf_register_power_amplifier_control2);
  uint8_t enable_fifo = 1 << 7;
  uint8_t transmitter_enable_on_time_symbol_bits = 3 << 3;
  uint8_t expected_buffer[] = {
          command, enable_fifo | transmitter_enable_on_time_symbol_bits
  };
  NetworkHardware_init(mrf);
  uint8_t offset_from_software_reset = 2;
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_buffer, buffer+offset_from_software_reset, 2);
}

void test_initSetsOtherControlRegistersAsSpecifiedInDatasheetExample(void) {
  uint8_t expected_buffer[] = {
          (uint8_t)(MRF_writeLongCommand(mrf_register_rf_control0) >> 8), (uint8_t)MRF_writeLongCommand(mrf_register_rf_control0), 0x03,
          (uint8_t)(MRF_writeLongCommand(mrf_register_rf_control1) >> 8), (uint8_t)MRF_writeLongCommand(mrf_register_rf_control1), 0x01,
          (uint8_t)(MRF_writeLongCommand(mrf_register_rf_control2) >> 8), (uint8_t)MRF_writeLongCommand(mrf_register_rf_control2), 0x80,
          (uint8_t)(MRF_writeLongCommand(mrf_register_rf_control6) >> 8), (uint8_t)MRF_writeLongCommand(mrf_register_rf_control6), 0x90,
          (uint8_t)(MRF_writeLongCommand(mrf_register_rf_control7) >> 8), (uint8_t)MRF_writeLongCommand(mrf_register_rf_control7), 0x80,
          (uint8_t)(MRF_writeLongCommand(mrf_register_rf_control8) >> 8), (uint8_t)MRF_writeLongCommand(mrf_register_rf_control8), 0x10,
          (uint8_t)(MRF_writeLongCommand(mrf_register_sleep_clock_control1) >> 8), (uint8_t)MRF_writeLongCommand(mrf_register_sleep_clock_control1), 0x21,
          MRF_writeShortCommand(mrf_register_base_band2), 0x80,
          MRF_writeShortCommand(mrf_register_energy_detection_threshold_for_clear_channel_assessment), 0x60,
          MRF_writeShortCommand(mrf_register_base_band6), 0x40
  };
  NetworkHardware_init(mrf);
  uint8_t number_of_previous_long_writes = 0;
  uint8_t number_of_previouse_short_writes = 3;
  int offset = number_of_previous_long_writes * 3 +
          number_of_previouse_short_writes * 2;
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_buffer, buffer+offset, 7*3+3*2);

}

void test_initEnablesRXInterruptForMRF(void) {
  uint8_t number_of_previous_short_writes = 6;
  uint8_t number_of_previous_long_writes = 7;
  int offset = number_of_previous_long_writes * 3 +
          number_of_previous_short_writes * 2;

  uint8_t reception_interrupt_enable_bit = (1 << 3);
  uint8_t expected_buffer[] = {
          MRF_writeShortCommand(mrf_register_interrupt_control), ~reception_interrupt_enable_bit
  };

  NetworkHardware_init(mrf);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_buffer, buffer+offset, 2);
}

void test_initSelectsChannelEleven(void) {
  uint8_t number_of_previous_short_writes = 7;
  uint8_t number_of_previous_long_writes = 7;
  int offset = number_of_previous_long_writes * 3 +
               number_of_previous_short_writes *2;
  uint8_t channel_eleven = 0x03;
  uint8_t expected_buffer[] = {
          (uint8_t)(MRF_writeLongCommand(mrf_register_rf_control0) >> 8), (uint8_t)MRF_writeLongCommand(mrf_register_rf_control0), channel_eleven
  };
  NetworkHardware_init(mrf);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_buffer, buffer+offset, 3);
}

void test_initSetsTransmitterPowerTo30dB(void) {
  uint8_t number_of_previouse_short_writes = 7;
  uint8_t number_of_previouse_long_writes = 8;
  int offset = number_of_previouse_long_writes * 3 +
               number_of_previouse_short_writes *2;
  uint8_t minus_thirty_db = 3 << 6;
  uint8_t expected_buffer[] = {
          (uint8_t)(MRF_writeLongCommand(mrf_register_rf_control3) >> 8),
          (uint8_t) MRF_writeLongCommand(mrf_register_rf_control3),
          minus_thirty_db
  };

  NetworkHardware_init(mrf);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_buffer, buffer+offset, 3);

}

void test_initResetsTheInternalStateMachine(void) {
  uint8_t number_of_previous_short_writes = 7;
  uint8_t number_of_previous_long_writes = 9;
  int offset = number_of_previous_long_writes * 3 +
               number_of_previous_short_writes * 2;
  uint8_t reset_state_machine = 0x04;
  uint8_t start_state_machine = 0x00;
  uint8_t expected_buffer[] = {
          MRF_writeShortCommand(mrf_register_rf_mode_control), reset_state_machine,
          MRF_writeShortCommand(mrf_register_rf_mode_control), start_state_machine
  };

  NetworkHardware_init(mrf);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_buffer, buffer+offset, 4);
}

void test_waitFor200MicroSecondsAfterInitialization(void) {
  NetworkHardware_init(mrf);
  TEST_ASSERT_EQUAL_DOUBLE(200, MockRuntime_lastDelayMicrosecondsArg());
}

void test_initMakesNoAsynchronousCallsToSPI(void) {
  NetworkHardware_init(mrf);
  TEST_ASSERT_EQUAL_UINT8(0, mock_interface.number_of_async_transfer_calls);
}

void test_ExceptionIsThrownWhenInterfaceIsBusyDuringInit(void) {
  mock_interface.isBusy = true;
  CEXCEPTION_T exception = 0;
  Try {
        NetworkHardware_init(mrf);
        TEST_FAIL();
      } Catch (exception) {
    TEST_ASSERT_EQUAL_UINT8(NETWORK_HARDWARE_IS_BUSY_EXCEPTION, exception);
  }
}

void test_initMakesCorrectNumberOfSyncCalls(void) {
  NetworkHardware_init(mrf);
  TEST_ASSERT_EQUAL_UINT8(18, mock_interface.number_of_sync_transfer_calls);
}
