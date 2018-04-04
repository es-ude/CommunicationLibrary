#include "MRFHelperFunctions.h"
#include "lib/src/MRFInternalConstants.h"

bool isLongAddress(uint16_t address){
  uint8_t last_short_control_register_address = 0x3F;
  return (address > last_short_control_register_address);
}

void MRF_setControlRegister(MRF *impl, uint16_t address, uint8_t value) {
  PeripheralInterface_selectPeripheral(impl->interface, impl->device);
  if (isLongAddress(address)) {
    uint8_t command[] = {
            MRF_writeLongCommandHighByte(address),
            MRF_writeLongCommandLowByte(address),
    };
    PeripheralInterface_writeBlocking(impl->interface, command, 2);
    PeripheralInterface_writeBlocking(impl->interface, &value, 1);

  } else {
    uint8_t command = MRF_writeShortCommand((uint8_t) address);
    PeripheralInterface_writeBlocking(impl->interface, &command, 1);
    PeripheralInterface_writeBlocking(impl->interface, &value, 1);
  }
  PeripheralInterface_deselectPeripheral(impl->interface, impl->device);
}

void MRF_writeBytesToShortRegisterAddress(MRF *impl, uint8_t address, const uint8_t *buffer, uint16_t size) {
  PeripheralInterface_selectPeripheral(impl->interface, impl->device);
  uint8_t command = MRF_writeShortCommand(address);
  PeripheralInterface_writeBlocking(impl->interface, &command, 1);
  PeripheralInterface_writeBlocking(impl->interface, buffer, size);
  PeripheralInterface_deselectPeripheral(impl->interface, impl->device);
}