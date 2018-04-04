#include "unity.h"
#include "lib/include/MockPeripheral.h"
#include "lib/src/MRFHelperFunctions.h"
#include "lib/src/MRFInternalConstants.h"

void test_setAddressForShortAddress(void){
  MRF mrf;
  uint8_t command = MRF_writeShortCommand(mrf_register_software_reset);
  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, &command, 1, 1);
  uint8_t value = 0xAB;
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, &value, 1, 1);
  PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
  MRF_setControlRegister(&mrf, mrf_register_software_reset, 0xAB);
}

void test_setAddressForLongAddress(void) {
  MRF mrf;
  uint8_t command[] = {
          MRF_writeLongCommandHighByte(mrf_register_rf_control6),
          MRF_writeLongCommandLowByte(mrf_register_rf_control6),
  };
  uint8_t value;
  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, command, 2, 2);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, &value, 1, 1);
  PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
  MRF_setControlRegister(&mrf, mrf_register_rf_control6, value);
}

void test_writeBytesToShortRegisterAddress(void) {
  MRF mrf;
  uint8_t command = MRF_writeShortCommand(mrf_register_short_address_low_byte);
  uint8_t buffer[10];
  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, &command, 1, 1);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, buffer, 10, 10);
  PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
  MRF_writeBytesToShortRegisterAddress(&mrf, mrf_register_short_address_low_byte, buffer, 10);
}