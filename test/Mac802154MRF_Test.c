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
  mrf = Mac802154_createMRF(inspected_memory, fakeDelay);
  mock_device.current_write_buffer_position = 0;
  mock_device.current_read_buffer_position = 0;
}

void tearDown(void) {
  Mac802154_destroy(mrf);
  TEST_ASSERT_EQUAL(0, MockMemoryManagement_numberOfAllocatedObjects(inspected_memory));
}

void test_initPerformsSetupLikeShownInDatasheet(void) {
  MRF *impl = (MRF*) mrf;
  MRF_setControlRegister_Expect(impl, mrf_register_software_reset, 0x07);
  MRF_setControlRegister_Expect(impl, mrf_register_power_amplifier_control2, 0x98);
  MRF_setControlRegister_Expect(impl, mrf_register_tx_stabilization, 0x95);
  MRF_setControlRegister_Expect(impl, mrf_register_rf_control0, 0x03);
  MRF_setControlRegister_Expect(impl, mrf_register_rf_control1, 0x01);
  MRF_setControlRegister_Expect(impl, mrf_register_rf_control2, 0x80);
  MRF_setControlRegister_Expect(impl, mrf_register_rf_control6, 0x90);
  MRF_setControlRegister_Expect(impl, mrf_register_rf_control7, 0x80);
  MRF_setControlRegister_Expect(impl, mrf_register_sleep_clock_control1, 0x21);
  MRF_setControlRegister_Expect(impl, mrf_register_base_band2, 0x80);
  MRF_setControlRegister_Expect(impl, mrf_register_energy_detection_threshold_for_clear_channel_assessment, 0x60);
  MRF_setControlRegister_Expect(impl, mrf_register_base_band6, 0x40);
  MRF_setControlRegister_Expect(impl, mrf_register_interrupt_control, (uint8_t) ~(1 << 3));
  fakeDelay_Expect(200);
  Mac802154_init(mrf, &mrf_config);
}


void test_initEnablesInterruptLineOnMRF(void) {
  TEST_IGNORE();
  uint8_t expected_buffer[] = {
          MRF_writeShortCommand(mrf_register_interrupt_control), (uint8_t) ~(1 << 3),
  };
  Mac802154_init(mrf, &mrf_config);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_buffer, write_buffer+30, 2);
}

void test_initSelectsTheCorrectChannel(void) {
  TEST_IGNORE();
  // conversion is derived from the data sheet
  uint8_t register_value = (uint8_t) ((mrf_config.channel % 10 - 1) * 3 + 0x03);
  uint8_t expected_buffer_for_channel11[] = {
          MRF_writeLongCommandHighByte(mrf_register_rf_control0), MRF_writeLongCommandLowByte(mrf_register_rf_control0),
          register_value,
  };
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_buffer_for_channel11, write_buffer+32, 3);
}