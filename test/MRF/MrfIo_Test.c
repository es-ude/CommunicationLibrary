#include "unity.h"
#include "src/Mac802154/MRF/MrfIo.h"
#include "PeripheralInterface/MockPeripheralInterface.h"
#include "src/Mac802154/MRF/MRFHelperFunctions.h"
#include "src/Mac802154/MRF/MRFInternalConstants.h"

void debug(const uint8_t *message) {}

void test_writeBlockingToLongAddress(void) {
  MrfIo mrf;
  uint8_t size = 2;
  uint8_t payload[size];
  uint16_t address = 19;
  uint8_t command[2] = {MRF_writeLongCommandFirstByte(address),
                        MRF_writeLongCommandSecondByte(address)};
  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, command, 2, 2);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, payload, size, size);
  PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
  MrfIo_writeBlockingToLongAddress(&mrf, payload, size, address);
}

void test_writeBlockingToShortAddress(void) {
  MrfIo mrf;
  uint8_t size = 5;
  uint8_t address = 1;
  uint8_t payload[5] = {0,1,2,3,4};
  uint8_t commands[size];
  for (uint8_t i=0; i<size; i++) {
    commands[i] = MRF_writeShortCommand(address+i);
  }
  for (uint8_t i=0; i<size; i++) {
    PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
    PeripheralInterface_writeBlocking_Expect(mrf.interface, commands+i, 1);
    PeripheralInterface_writeBlocking_Expect(mrf.interface, payload+i, 1);
    PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
  }
  MrfIo_writeBlockingToShortAddress(&mrf, payload, size, address);
}

void check_setControlRegister(uint16_t register_address, const uint8_t *command, uint8_t command_length) {
  MrfIo mrf;
  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, command, command_length, command_length);
  uint8_t value;
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, &value, 1, 1);
  PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
  MrfIo_setControlRegister(&mrf, register_address, value);
}

void test_setShortAddressRegisterValue(void){
  uint8_t command = MRF_writeShortCommand(mrf_register_software_reset);
  check_setControlRegister(mrf_register_software_reset, &command, 1);
}

void test_setLongAddressRegisterValue(void) {
  uint8_t command[] = {
          MRF_writeLongCommandFirstByte(mrf_register_rf_control6),
          MRF_writeLongCommandSecondByte(mrf_register_rf_control6),
  };
  check_setControlRegister(mrf_register_rf_control6, command, 2);
}

void check_readAddressControlRegister(uint16_t register_address, const uint8_t *command, uint8_t command_length) {
  MrfIo mrf;
  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, command, command_length, command_length);
  uint8_t value = 0xAB;
  PeripheralInterface_readBlocking_Expect(mrf.interface, &value, 1);
  PeripheralInterface_readBlocking_IgnoreArg_buffer();
  PeripheralInterface_readBlocking_ReturnThruPtr_buffer(&value);
  PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
  uint8_t expected = MrfIo_readControlRegister(&mrf, register_address);
  TEST_ASSERT_EQUAL_HEX8(expected, value);
}

void test_readShortAddressControlRegister(void) {
  uint8_t command = MRF_readShortCommand(mrf_register_interrupt_status);
  check_readAddressControlRegister(mrf_register_interrupt_status, &command, 1);
}

void test_readLongAddressControlRegister(void) {
  uint8_t command[] = {MRF_readLongCommandFirstByte(mrf_register_rf_control6),
          MRF_readLongCommandSecondByte(mrf_register_rf_control6)};
  check_readAddressControlRegister(mrf_register_rf_control6, command, 2);
}

void test_readBlockingFromLongAddress(void) {
  MrfIo mrf;
  uint8_t buffer;
  uint8_t expected_buffer = 0xC9;
  uint8_t command[] = {
          MRF_readLongCommandFirstByte(mrf_rx_fifo_start),
          MRF_readLongCommandSecondByte(mrf_rx_fifo_start),
  };
  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, command, 2, 2);
  PeripheralInterface_readBlocking_ExpectWithArray(mrf.interface, 1, &buffer, 1, 1);
  PeripheralInterface_readBlocking_ReturnThruPtr_buffer(&expected_buffer);
  PeripheralInterface_readBlocking_IgnoreArg_buffer();
  PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
  MrfIo_readBlockingFromLongAddress(&mrf, mrf_rx_fifo_start, &buffer, 1);
  TEST_ASSERT_EQUAL_HEX8(expected_buffer, buffer);
}

void test_readBlockingFromLongAddress2(void) {
  MrfIo mrf;
  uint8_t buffer[3];
  uint8_t expected_buffer[] = {0xC9, 0xAB, 0x14};
  uint8_t command[] = {
          MRF_readLongCommandFirstByte(mrf_rx_fifo_start),
          MRF_readLongCommandSecondByte(mrf_rx_fifo_start),
  };
  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, command, 2, 2);
  PeripheralInterface_readBlocking_ExpectWithArray(mrf.interface, 1, &buffer, 3, 3);
  PeripheralInterface_readBlocking_ReturnThruPtr_buffer(&expected_buffer);
  PeripheralInterface_readBlocking_IgnoreArg_buffer();
  PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
  MrfIo_readBlockingFromLongAddress(&mrf, mrf_rx_fifo_start, &buffer, 3);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_buffer, buffer, 3);
}
