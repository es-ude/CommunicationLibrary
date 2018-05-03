#include "unity.h"
#include "lib/src/Mac802154/MRF/MrfIo.h"
#include "lib/include/MockPeripheral.h"

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
  uint8_t address = 15;
  uint8_t payload[size];
  uint8_t command[1] = {MRF_writeShortCommand(address)};
  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, command, 1, 1);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, payload, size, size);
  PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
  MrfIo_writeBlockingToShortAddress(&mrf, payload, size, address);
}