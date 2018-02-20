#include <string.h>
#include "lib/include/Mac802154.h"
#include "test/Mocks/MockSPIImpl.h"
#include "test/Mocks/MockRuntimeLibraryImpl.h"
#include "lib/src/MRFInternalConstants.h"
#include "lib/src/MRFHelperFunctions.h"
#include "lib/include/Exception.h"

#define UNITY_INCLUDE_DOUBLE
#include "src/unity.h"
#include "CException.h"
#include "lib/include/Mac802154MRFImpl.h"


#define MEMORY_CAPACITY sizeof(Mac802154) + sizeof(SPISlave) + 64
static uint8_t raw_memory[MEMORY_CAPACITY];

static MockAllocateConfig allocate_config = {.returned_address = raw_memory};
static SPIDeviceMockImpl mock_interface;
static uint8_t slave_select_line;
static SPISlave output_device = {
        .hw_interface = (SPI *) &mock_interface,
        .slave_select_register = &slave_select_line,
        .slave_select_pin = 1,
};
static Mac802154 *mrf;
static Mac802154Config config;
static uint8_t buffer[128];
static SPIMessage spi_message_buffer[40];

static void setUpNetworkHardwareConfig(Mac802154Config *config);

void setUp(void) {
  MockAllocate_configure(&allocate_config);
  SPIDeviceMockImpl_init(&mock_interface);
  mock_interface.input_buffer = NULL;
  mock_interface.output_buffer = buffer;
  mock_interface.message_buffer = spi_message_buffer;
  mrf = Mac802154_createMRF(&output_device, MockAllocate_allocate, MockRuntime_delayMicroseconds);
  setUpNetworkHardwareConfig(&config);
  memset(buffer, 0, 128);
}

void test_initPerformsSoftwareReset(void) {
  uint8_t expected_buffer[3] = {
          MRF_writeShortCommand(mrf_register_software_reset),
          0x07};
  SPIMessage expected_message = {.length = 2, .outgoing_data = expected_buffer, .incoming_data = NULL};
  Mac802154_init(mrf, &config);
  TEST_ASSERT_TRUE(SPIDeviceMockImpl_messageWasTransferred(&mock_interface, &expected_message));
}

void test_initSetsTXStabilizationRegisterToRecommendedValue(void) {
  uint8_t expected_buffer[2] = {
          ((mrf_register_tx_stabilization << 1) & ~(1 << 7)) | 1,
          0x09,
  };
  Mac802154_init(mrf, &config);
  SPIMessage message = {.length = 2, .incoming_data = NULL, .outgoing_data = expected_buffer};

  TEST_ASSERT_TRUE(SPIDeviceMockImpl_messageWasTransferred(&mock_interface, &message));
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
  SPIMessage expected_message = {
          .length = 2,
          .outgoing_data = expected_buffer,
  };
  Mac802154_init(mrf, &config);
  TEST_ASSERT_TRUE(SPIDeviceMockImpl_messageWasTransferred(&mock_interface, &expected_message));
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
  Mac802154_init(mrf, &config);
  uint8_t number_of_previous_long_writes = 0;
  uint8_t number_of_previous_short_writes = 3;
  int offset = number_of_previous_long_writes * 3 +
          number_of_previous_short_writes * 2;
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_buffer, buffer+offset, 7*3+3*2);

}

void test_initEnablesRXInterruptForMRF(void) {

  uint8_t reception_interrupt_enable_bit = (1 << 3);
  uint8_t expected_buffer[] = {
          MRF_writeShortCommand(mrf_register_interrupt_control), ~reception_interrupt_enable_bit
  };
  SPIMessage message = {
          .length = 2,
          .outgoing_data = expected_buffer,
          .incoming_data = NULL
  };
  Mac802154_init(mrf, &config);
  TEST_ASSERT_TRUE(SPIDeviceMockImpl_messageWasTransferred(&mock_interface, &message));
}

void test_initSelectsChannelEleven(void) {
  uint8_t channel_eleven = 0x03;
  uint8_t expected_buffer[] = {
          (uint8_t)(MRF_writeLongCommand(mrf_register_rf_control0) >> 8), (uint8_t)MRF_writeLongCommand(mrf_register_rf_control0), channel_eleven
  };
  SPIMessage message = {
          .length = 3,
          .outgoing_data = expected_buffer,
          .incoming_data = NULL
  };
  Mac802154_init(mrf, &config);
  TEST_ASSERT_TRUE(SPIDeviceMockImpl_messageWasTransferred(&mock_interface, &message));
}

void test_initSetsTransmitterPowerTo30dB(void) {

  uint8_t minus_thirty_db = 3 << 6;
  uint8_t expected_buffer[] = {
          (uint8_t)(MRF_writeLongCommand(mrf_register_rf_control3) >> 8),
          (uint8_t) MRF_writeLongCommand(mrf_register_rf_control3),
          minus_thirty_db
  };
  SPIMessage expected_message = {
          .length = 3,
          .outgoing_data = expected_buffer,
          .incoming_data = NULL,
  };
  Mac802154_init(mrf, &config);
  TEST_ASSERT_TRUE(SPIDeviceMockImpl_messageWasTransferred(&mock_interface, &expected_message));
}

void test_initResetsTheInternalStateMachine(void) {
  uint8_t reset_state_machine = 0x04;
  uint8_t start_state_machine = 0x00;
  uint8_t expected_buffer[] = {
          MRF_writeShortCommand(mrf_register_rf_mode_control), reset_state_machine,
          MRF_writeShortCommand(mrf_register_rf_mode_control), start_state_machine
  };

  SPIMessage message = {
          .length = 2,
          .outgoing_data = expected_buffer,
          .incoming_data = NULL,
  };
  Mac802154_init(mrf, &config);
  TEST_ASSERT_TRUE(SPIDeviceMockImpl_messageWasTransferred(&mock_interface, &message));
  message.outgoing_data = expected_buffer + 2;
  TEST_ASSERT_TRUE(SPIDeviceMockImpl_messageWasTransferred(&mock_interface, &message));

}

void test_waitFor200MicroSecondsAfterInitialization(void) {
  Mac802154_init(mrf, &config);
  TEST_ASSERT_EQUAL_DOUBLE(200, MockRuntime_lastDelayMicrosecondsArg());
}

void test_initMakesNoAsynchronousCallsToSPI(void) {
  Mac802154_init(mrf, &config);
  TEST_ASSERT_EQUAL_UINT8(0, mock_interface.number_of_async_transfer_calls);
}

void test_ExceptionIsThrownWhenInterfaceIsBusyDuringInit(void) {
  mock_interface.isBusy = true;
  CEXCEPTION_T exception = 0;
  Try {
        Mac802154_init(mrf, &config);
        TEST_FAIL();
      } Catch (exception) {
    TEST_ASSERT_EQUAL_UINT8(NETWORK_HARDWARE_IS_BUSY_EXCEPTION, exception);
  }
}

void test_PanIdIsSetDuringInitialization(void) {
  config.pan_id = 0xABCD;
  uint8_t expected[] = {
          MRF_writeShortCommand(mrf_register_pan_id_low_byte), 0xCD, 0xAB,
  };
  SPIMessage message = {
          .length = 3,
          .outgoing_data = expected,
          .incoming_data = NULL,
  };
  Mac802154_init(mrf, &config);
  TEST_ASSERT_TRUE(SPIDeviceMockImpl_messageWasTransferred(&mock_interface, &message));
}

void test_ShortSourceAddressIsSetDuringInitialization(void) {
  config.short_source_address = 0xABDC;
  uint8_t expected[] = {
          MRF_writeShortCommand(mrf_register_short_address_low_byte),
          0xDC,
          0xAB,
  };
  SPIMessage message = {
          .length = 3,
          .outgoing_data = expected,
          .incoming_data = NULL,
  };
  Mac802154_init(mrf, &config);
  TEST_ASSERT_TRUE(SPIDeviceMockImpl_messageWasTransferred(&mock_interface, &message));
}

void test_ExtendedSourceAddressIsSetDuringInitialization(void) {
  config.extended_source_address[3] = 0xA5;
  config.extended_source_address[6] = 0xD6;
  uint8_t expected[9];
  expected[0] = MRF_writeShortCommand(mrf_register_extended_address0);
  memcpy(expected+1, config.extended_source_address, 8);
  SPIMessage message = {
          .length = 9,
          .outgoing_data = expected,
          .incoming_data = NULL,
  };
  Mac802154_init(mrf, &config);
  TEST_ASSERT_TRUE(SPIDeviceMockImpl_messageWasTransferred(&mock_interface, &message));
}

void test_sendWithoutSettingAnything(void) {
  TEST_IGNORE();
  FrameHeader802154 header = {
          .control.as_struct = {
                  .frame_type = 0b001,
                  .security_enabled = 0b0,
                  .frame_pending = 0b0,
                  .acknowledgment_request = 0b0,
                  .pan_id_compression = 0b1,
                  .reserved = 0b0,
                  .sequence_number_suppression = 0b0,
                  .information_element_present = 0b0,
                  .destination_addressing_mode = 0b10,
                  .frame_version = 0b10,
                  .source_addressing_mode = 0b10,
          },
          .destination.short_address = {0x00, 0x00},
          .destination_pan_id = {0xff, 0xff},
          .sequence_number = 0,
  };
  SPIMessage header_part = {
          .length = sizeof(FrameHeader802154) - 6,
          .outgoing_data = (uint8_t*) &header,
          .incoming_data = NULL,
          .next = NULL,
  };
  uint8_t message_and_header_size[] = {0x07, 0x07};
  SPIMessage message_and_header_size_part = {
          .length = 2,
          .outgoing_data = message_and_header_size,
          .incoming_data = NULL,
          .next = NULL,
  };
  Mac802154_send(mrf);
  TEST_ASSERT_TRUE(SPIDeviceMockImpl_messageWasTransferred(&mock_interface, &message_and_header_size_part));
}

void setUpNetworkHardwareConfig(Mac802154Config *config) {
  config->pan_id = 0xffff;
  config->short_source_address = 0xffff;
  for (int i=0; i<8; i++) {
    config->extended_source_address[i] = 0xff;
  }
}



